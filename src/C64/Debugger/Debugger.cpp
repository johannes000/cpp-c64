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

	ImGui::Text("$%02X %s %s ", inst.OpCode, inst.Name.c_str(), cpu->GetCurrentInstructionAddressString().c_str());

	ImGui::Text("A: $%02X [%03d]", stats.A, stats.A);
	ImGui::Text("X: $%02X [%03d]", stats.X, stats.X);
	ImGui::Text("Y: $%02X [%03d]", stats.Y, stats.Y);
	ImGui::Text("PC: $%04X [%05d]", stats.PC, stats.PC);
	ImGui::Text("SP: $%02X [%03d]", stats.SP, stats.SP);
	ImGui::Text("PS: $%02X [%03d]", stats.PS.data, stats.PS.data);

	ImGui::Text("Flags:");
	ImGui::Text("C: %d, Z: %d, I: %d, D: %d, B: %d, V: %d, N: %d",
				stats.PS.C, stats.PS.Z, stats.PS.I, stats.PS.D,
				stats.PS.B, stats.PS.V, stats.PS.N);

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
