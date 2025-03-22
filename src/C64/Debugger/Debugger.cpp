#include "Debugger.hpp"
#include "C64\C64.hpp"

namespace {
constexpr ImVec4 BG_COLOR = {0.8f, 0.8f, 0.8f, 1.0f};
constexpr ImVec4 CYAN = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
constexpr ImVec4 MAGENTA = ImVec4(1.0f, 0.502f, 0.957f, 1.0f);
constexpr ImVec4 YELLOW = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
constexpr ImVec4 RED = ImVec4(1.0f, 0.149f, 0.447f, 1.0f);
constexpr ImVec4 GREEN = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
constexpr ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
constexpr ImVec4 DARK_GRAY = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}; // namespace

Debugger::Debugger(C64 *c64) : mC64(c64) {
	log = spdlog::get("DEB");
}

void Debugger::RenderDebugger() {
	ShowC64CpuControls();
	ShowC64CpuStats();
	ShowDecompiler();
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

	auto sptext = [](const char *t) {
		ImGui::TableNextColumn();
		ImGui::Text(t);
	};
	auto spdata = [](u8 data) {
		ImGui::TableNextColumn();
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

void Debugger::ShowDecompiler() {
	if (mKernalDecon.size() == 0)
		DecompileKernal();

	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
	static bool followPC = true;
	static char gotoAddrInput[5] = "";

	if (ImGui::Begin("Decompiler")) {
		ImGui::Checkbox("Follow PC", &followPC);
		ImGui::SameLine();
		if (ImGui::Button("Goto PC")) {
			followPC = true;
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(100);
		if (ImGui::InputText("Goto Address", gotoAddrInput, sizeof(gotoAddrInput), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue)) {
			u16 addr = static_cast<u16>(strtoul(gotoAddrInput, nullptr, 16));
			if (addr >= KERNAL_ADDR && addr <= KERNAL_ADDR + KERNAL_SIZE - 1) {
				followPC = false;
			}
		}
		ImGui::PopItemWidth();

		ImGui::BeginChild("DecompilerTable", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginTable("Decompiler", 7, flags)) {
			ImGui::TableSetupColumn("PC", ImGuiTableColumnFlags_WidthFixed, 0.0f);
			ImGui::TableSetupColumn("Adresse", ImGuiTableColumnFlags_WidthFixed, 0.0f);
			ImGui::TableSetupColumn("OpCode", ImGuiTableColumnFlags_WidthFixed, 0.0f);
			ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 0.0f);
			ImGui::TableSetupColumn("Zyklen", ImGuiTableColumnFlags_WidthFixed, 0.0f);
			ImGui::TableSetupColumn("Mnemonik", ImGuiTableColumnFlags_WidthFixed, 0.0f);
			ImGui::TableSetupColumn("Relative Adresse", ImGuiTableColumnFlags_WidthFixed, 0.0f);

			ImGuiListClipper clipper;
			clipper.Begin(mKernalDecon.size());

			while (clipper.Step()) {
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
					const auto &inst = mKernalDecon[i];
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					if (inst.addr == mC64->GetCpu()->GetPC()) {
						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(RED));
						if (followPC)
							ImGui::SetScrollHereY();
						ImGui::Text(">");
					} else {
						ImGui::Text(" ");
					}

					ImGui::TableNextColumn();
					ImGui::Text("%04X", inst.addr);

					ImGui::TableNextColumn();
					ImGui::Text("%02X", inst.opCode);

					ImGui::TableNextColumn();
					switch (inst.length) {
						case 1:
							ImGui::Text(" ");
							break;
						case 2:
							ImGui::Text("%02X", inst.parameter);
							break;
						case 3:
							ImGui::Text("%04X", inst.parameter);
							break;
						default:
							ImGui::Text(" ");
							break;
					}

					ImGui::TableNextColumn();
					ImGui::Text("%i", inst.cycles);

					ImGui::TableNextColumn();
					ImGui::Text("%s", inst.mnemonic.c_str());

					ImGui::TableNextColumn();
					ImGui::Text("%s ", inst.relAddr.c_str());
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();
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

void Debugger::DecompileKernal() {
	log->info("Decompiling Kernal");
	if (mKernalDecon.size() > 0)
		return;
	auto cpu = mC64->GetCpu();
	auto kernal = mC64->GetKernalPointer();
	u16 addr = 0u;

	while (addr < KERNAL_SIZE) {
		auto opCode = kernal->IOLoad8(addr);
		auto inst = cpu->GetInstruction(opCode);
		DebuggerDeconInst deconstructedInst{
			.addr = (u16)(addr + KERNAL_ADDR),
			.opCode = inst.OpCode,
			.length = inst.length,
			.cycles = inst.cycles,
			.parameter = 0, // TODO
			.relAddr = "",	// TODO
			.mnemonic = inst.Name,
		};
		mKernalDecon.push_back(deconstructedInst);
		addr += inst.length;
	}
	log->info("Decompiling Kernal done");
}
