#include "Debugger.hpp"
#include "C64\C64.hpp"

Debugger::Debugger(C64 *c64) : mC64(c64) {
}

void Debugger::RenderDebugger() {
	ShowC64CpuControls();
	ShowC64CpuStats();
	ShowMemoryView();
}

void Debugger::ShowC64CpuControls() {
	ImGui::Begin("C64 Controls");
	if (ImGui::Button("Reset")) {
		mC64->Reset();
	}
	if (ImGui::Button("Step")) {
		mC64->RunOneInstruction();
	}
	ImGui::End();
}

void Debugger::ShowC64CpuStats() {
	auto cpu = mC64->GetCpu();
	auto stats = cpu->GetCpuState();
	auto inst = cpu->GetCurrentInstrution();

	ImGui::Begin("6502 Stats");

	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	auto f = [](const char *name, const char *hex, const char *dec, auto data) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", name);
		ImGui::TableSetColumnIndex(1);
		ImGui::Text(hex, data);
		ImGui::TableSetColumnIndex(2);
		ImGui::Text(dec, data);
	};

	if (ImGui::BeginTable("Registers", 3, flags)) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("$%02X", inst.OpCode);
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%s", inst.Name.c_str());
		ImGui::TableSetColumnIndex(2);
		ImGui::Text("%s", cpu->GetCurrentInstructionAddressString().c_str());

		f("PC", "$%04X", "[%04d]", stats.PC);
		f("A", "$%02X", "[%04d]", stats.A);
		f("X", "$%02X", "[%04d]", stats.X);
		f("Y", "$%02X", "[%04d]", stats.Y);
		f("SP", "$%02X", "[%04d]", stats.SP);
		f("PS", "$%02X", "[%04d]", stats.PS.data);
		ImGui::EndTable();
	}

	i32 spalte = 0;
	auto sptext = [&spalte](const char *t) {
		ImGui::TableSetColumnIndex(spalte++);
		ImGui::Text(t);
	};
	auto spdata = [&spalte](u8 data) {
		ImGui::TableSetColumnIndex(spalte++);
		ImGui::Text("%d", data);
	};

	if (ImGui::BeginTable("PS", 7, flags)) {
		ImGui::TableNextRow();
		sptext("C");
		sptext("Z");
		sptext("I");
		sptext("D");
		sptext("B");
		sptext("V");
		sptext("N");
		spalte = 0;
		ImGui::TableNextRow();
		spdata(stats.PS.C);
		spdata(stats.PS.Z);
		spdata(stats.PS.I);
		spdata(stats.PS.D);
		spdata(stats.PS.B);
		spdata(stats.PS.V);
		spdata(stats.PS.N);
		ImGui::EndTable();
	}

	ImGui::End();
}
void Debugger::ShowMemoryView() {
	if (ImGui::Begin("Memory")) {
		auto kernalRom = mC64->GetKernalData();
		auto basicRom = mC64->GetBasicRomData();
		auto charSetRom = mC64->GetCharSetData();
		auto ram = mC64->GetRamData();

		ImGui::Text("RAM1");
		ShowMemoryRange("RAM1", ram, 0x0000, RAM_SIZE);
		ImGui::Text("Basic");
		ShowMemoryRange("Basic", basicRom, 0xA000, BASIC_ROM_SIZE);
		ImGui::Text("Charset");
		ShowMemoryRange("Charset", charSetRom, 0xD000, CHARACTER_ROM_SIZE);
		ImGui::Text("Kernal");
		ShowMemoryRange("Kernal", kernalRom, 0xE000, KERNAL_SIZE);

		ImGui::End();
	}
}

void Debugger::ShowMemoryRange(const char *label, const u8 *data, u16 startAddress, u16 size) {
	if (ImGui::CollapsingHeader(label)) {
		const i32 bytesPerRow = 16;
		const i32 mitte = 7;

		ImGui::BeginChild(label, ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		ImGuiListClipper clipper;
		clipper.Begin((size + bytesPerRow - 1) / bytesPerRow);

		while (clipper.Step()) {
			for (i32 row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
				u16 address = startAddress + row * bytesPerRow;

				ImGui::Text("%04X: ", address);

				for (i32 j = 0; j < bytesPerRow; ++j) {
					u16 index = row * bytesPerRow + j;
					if (index < size) {
						ImGui::SameLine(0.0f, 1.0f);
						ImGui::Text("%02X", data[index]);
					}
					if (j == mitte) {
						ImGui::SameLine(0.0f, 1.0f);
						ImGui::Text(" ");
					}
				}

				ImGui::SameLine(0.0f, 1.0f);
				ImGui::Text("|");

				for (i32 j = 0; j < bytesPerRow; ++j) {
					u16 index = row * bytesPerRow + j;
					if (index < size) {
						u8 byte = data[index];
						char c = (byte >= 32 && byte < 127) ? static_cast<char>(byte) : '.';
						ImGui::SameLine(0.0f, 1.0f);
						ImGui::Text("%c", c);
					}
					if (j == mitte) {
						ImGui::SameLine(0.0f, 1.0f);
						ImGui::Text(" ");
					}
				}
			}
		}

		ImGui::EndChild();
	}
}
