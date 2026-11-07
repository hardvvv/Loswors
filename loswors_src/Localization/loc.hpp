#pragma once
#include <cstdint>
#include <string>

class LocEditor;

namespace loc {

	class txt {
	private:
		const uint64_t key;
		mutable std::string text;

		static constexpr uint64_t GetHash(const char* text) {
			// FNV-1a hash (64-bit)
			constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
			constexpr uint64_t FNV_PRIME = 1099511628211ULL;

			if (!text) return 0;

			uint64_t hash = FNV_OFFSET_BASIS;
			while (*text != '\0') {
				hash ^= static_cast<uint64_t>(static_cast<uint8_t>(*text));
				hash *= FNV_PRIME;
				++text;
			}
			return hash;
		}

		friend class ::LocEditor;

		void SetVal(std::string t) const {
			text = t;
		}
		
	public:
		explicit txt(const char* infilekey, const char* defV) : key(GetHash(infilekey)), text(defV) {}

		constexpr bool IsCompared(const char* infilekey) const {
			return (key == GetHash(infilekey));
		}

		txt& operator = (const txt&) = delete;
		txt& operator = (const char*) = delete;

		// get while call
		operator const char* () const { return text.c_str(); }
		operator const std::string& () const { return text; }
	};


	//      _____        _      ___               
	//     |_   _|____ _| |_   / __|_ _ _  _ _ __ 
	//       | |/ -_) \ /  _| | (_ | '_| || | '_ \
	//       |_|\___/_\_\\__|  \___|_|  \_,_| .__/
	// ------------------------------------ |_| -----------------------------------------------------------------------

	struct StartST {
		struct i_header {
			txt nofile{		"StartST.header.nofile",		"No file exit" };
			txt forgetpath{	"StartST.header.forgetpath",	"Forget path" };
			txt fcount{		"StartST.header.fcount",		"File count" };
			txt newbtn{		"StartST.header.newbtn",		"CREATE NEW" };
			txt closebtn{	"StartST.header.closebtn",		"Close (esc)" };
		};
		struct i_newf {
			txt name{		"StartST.newf.name",	"NAME:" };
		};
		struct i_passz {
			txt pass{			"StartST.passz.pass",			"Password:" };
			txt passexpln{		"StartST.passz.passexpln",		"This password - part of the file encryption algorithm" };
			txt enterpass{		"StartST.passz.enterpass",		"Enter new password:" };
			txt confirmpass{	"StartST.passz.confirmpass",	"Confirm new password:" };
		};
		// ------------------------------------------------
		i_header header;
		i_newf newf;
		i_passz passz;
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct MainST {
		struct i_header {
			txt search{		"MainST.header.search",		"Search by:" };
			txt name{		"MainST.header.name",		"Name" };
			txt desc{		"MainST.header.desc",		"Description" };
			txt savebtn{	"MainST.header.savebtn",	"SAVE" };
			txt fsettbtn{	"MainST.header.fsettbtn",	"File Param" };
		};
		struct i_PUs {
			txt notfound{	"MainST.PUs.notfound",	"Pass Units not found" };
			txt newbtn{		"MainST.PUs.newbtn",	"ADD NEW" };
		};
		struct i_PINconfirm {
			txt enterpin{	"MainST.PINconfirm.enterpin",	"Enter PIN" };
			txt warning{	"MainST.PINconfirm.warning",	"If password or PIN was entered incorrectly, saving will not start !!" };
		};
		struct i_FparFcpy {
			txt fparambtn{		"MainST.FparFcpy.fparambtn",	"File Param" };
			txt fcopybtn{		"MainST.FparFcpy.fcopybtn",		"Copy File" };
			txt warning0{		"MainST.FparFcpy.warning0",		"Old password is compared with the file-opening password" };
			txt warning1{		"MainST.FparFcpy.warning1",		"If PIN error, opening password may be incorrect" };
			txt fname{			"MainST.FparFcpy.fname",		"Name:" };
			txt twofile{		"MainST.FparFcpy.twofile",		"2 File" };
			txt oldz{			"MainST.FparFcpy.oldz",			"OLD" };
			txt newz{			"MainST.FparFcpy.newz",			"NEW" };
			txt oldpin{			"MainST.FparFcpy.oldpin",		"Old PIN:" };
			txt newpin{			"MainST.FparFcpy.newpin",		"New PIN:" };
			txt oldpassconf{	"MainST.FparFcpy.oldpassconf",	"Confirm old password:" };
			txt newpassenter{	"MainST.FparFcpy.newpassenter",	"Enter new Password:" };
			txt newpassconf{	"MainST.FparFcpy.newpassconf",	"Confirm new Password:" };
		};
		struct i_editPU {
			txt name{		"MainST.EditPU.name",		"Name:" };
			txt pass{		"MainST.EditPU.pass",		"Password:" };
			txt desc{		"MainST.EditPU.desc",		"Description:" };
			txt closebtn{	"MainST.EditPU.closebtn",	"CLOSE" };
			txt okbtn{		"MainST.EditPU.okbtn",		"OK" };
		};
		// ------------------------------------------------
		i_header header;
		i_PUs PUs;
		i_PINconfirm PINconfirm;
		i_FparFcpy FparFcpy;
		i_editPU editPU;
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct SettingsPN {
		txt forgetpath{		"Settings.forgetpath",		"Forget all path after exit" };
		txt nofileexit{		"Settings.nofileexit",		"Exit to Start state if no file in path" };
		txt decodeanim{		"Settings.decodeanim",		"Show Decode Animation" };
		txt randomtheme{	"Settings.randomtheme",		"Random UI Theme" };
		txt inacttimeout{	"Settings.inacttimeout",	"Inactive Timeout:" };
		txt sec{			"Settings.sec",				"s." };
		txt selectlang{		"Settings.selectlang",		"Select language:" };
		txt setdefwh{		"Settings.setdefwh",		"Set Default" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct DownBarPN {
		txt closeesc{ "Downbar.closeesc", "Close (esc)" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct Drager {
		txt nodrop{	"Drager.nodrop",	"Not allow to drop file now !" };
		txt f0{		"Drager.f0",		"File 0" };
		txt f1{		"Drager.f1",		"File 1" };
		txt onef{	"Drager.onef",		"Drop File (if 2 files -> open 2 file mode)" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct FileInfo {
		txt header{		"FileInfo.header",		"File Param" };
		txt version{	"FileInfo.version",		"File Version:" };
		txt size{		"FileInfo.size",		"File Size:" };
		txt PUcount{	"FileInfo.PUcount",		"Pass Unit Count:" };
		txt opentime{	"FileInfo.opentime",	"Time From Open:" };
		txt entropy{	"FileInfo.entropy",		"Private Data Entropy 0-8:" };
		txt Hdiv{		"FileInfo.Hdiv",		"--- File open time ---" };
		txt totalsec{	"FileInfo.totalsec",	"Total:" };
		txt gxor{		"FileInfo.gxor",		"Global | XOR & Suffle:" };
		txt ixor{		"FileInfo.ixor",		"Inside | XOR & Suffle:" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct PassGen {
		txt length{ "PassGen.length", "Length:" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct PUBlock {
		txt okbtn{			"PUBlock.okbtn",		"OK" };
		txt canselbtn{		"PUBlock.canselbtn",	"Cancel" };
		txt delquestion{	"PUBlock.delquestion",	"Delete Pass Unit?" };
		txt name{			"PUBlock.name",			"Name" };
		txt password{		"PUBlock.password",		"Password" };
		txt description{	"PUBlock.description",	"Description" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct Notif {
		struct ii_sv {
			txt crtdone0{				"Notif.core.sv.crtdone0",			"Create Done" };
				txt crtdone1{			"Notif.core.sv.crtdone1",			"Save file was successfully created. See log" };
			txt savedone0{				"Notif.core.sv.savedone0",			"Save Done" };
				txt savedone1{			"Notif.core.sv.savedone1",			"Data written successfully. See log" };
			txt errdirnotexist0{		"Notif.core.sv.errdirnotexist0",	"Create File Error" };
				txt errdirnotexist1{	"Notif.core.sv.errdirnotexist1",	"Invalid directory path" };
			txt errfileexis0{			"Notif.core.sv.errfileexis0",		"Create File Error" };
				txt errfileexis1{		"Notif.core.sv.errfileexis1",		"File allready exist" };
			txt errsave0{				"Notif.core.sv.errsave0",			"Save Error" };
				txt errsave1{			"Notif.core.sv.errsave1",			"Error while trying to save file. See log" };
		};
		struct ii_inptverf {
			txt newpasserr0{			"Notif.core.inptverf.newpasserr0",		"NEW Password Error" };
				txt newpasserr1{		"Notif.core.inptverf.newpasserr1",		"(Enter|Confirm) Passwords don't match. See log" };
			txt oldpasserr0{			"Notif.core.inptverf.oldpasserr0",		"OLD Password Error" };
				txt oldpasserr1{		"Notif.core.inptverf.oldpasserr1",		"OLD Password is incorrect\nIt doesn`t match previous input" };
			txt pinasciierr0{			"Notif.core.inptverf.pinasciierr0",		"PIN Error" };
				txt pinasciierr1{		"Notif.core.inptverf.pinasciierr1",		"PIN Unallowed symbol. See log" };
			txt newpinasciierr0{		"Notif.core.inptverf.newpinasciierr0",	"NEW PIN Error" };
				txt newpinasciierr1{	"Notif.core.inptverf.newpinasciierr1",	"NEW PIN Unallowed symbol. See log" };
			txt pinhasherr0{			"Notif.core.inptverf.pinhasherr0",		"Incorrect PIN" };
				txt pinhasherr1{		"Notif.core.inptverf.pinhasherr1",		"Wrong PIN was entered. Hash error" };
		};
		struct ii_tryentr {
			txt errfread0{			"Notif.core.tryentr.errfread0",		"Read File Error" };
				txt errfread1{		"Notif.core.tryentr.errfread1",		"Error while reading data from file by path. See log" };
			txt warnfdownver0{		"Notif.core.tryentr.warnfdownver0",	"Down Version File" };
				txt warnfdownver1{	"Notif.core.tryentr.warnfdownver1",	"File has lower version than the current program" };
			txt errfupver0{			"Notif.core.tryentr.errfupver0",	"Upper Version File" };
				txt errfupver1{		"Notif.core.tryentr.errfupver1",	"File has upper version or incorrect format" };
			txt errfsize0{			"Notif.core.tryentr.errfsize0",		"Incorrect File Size" };
				txt errfsize1{		"Notif.core.tryentr.errfsize1",		"File has invalid size" };
			txt errfformat0{		"Notif.core.tryentr.errfformat0",	"Incorrect File Format" };
				txt errfformat1{	"Notif.core.tryentr.errfformat1",	"File has incorrect format" };
			txt errargon0{			"Notif.core.tryentr.errargon0",		"Argon2 ERROR" };
				txt errargon1{		"Notif.core.tryentr.errargon1",		"Error while running Argon2 KDF algorithm" };
		};
		struct i_core {
			ii_sv sv;
			ii_inptverf inptverf;
			ii_tryentr tryentr;
			//  -------------
			txt unsavewarn0{		"Notif.core.unsavewarn0",	"Unsaved changes!" };
				txt unsavewarn1{	"Notif.core.unsavewarn1",	"You cannot work with unsaved changes" };
			txt changelost0{		"Notif.core.changelost0",	"Changes Lost!" };
				txt changelost1{	"Notif.core.changelost1",	"Exit without save" };
		};
		// ------------------------------------------------
		i_core core;

		txt copydone0{		"Notif.copydone0",	"Copy Done!" };
			txt copydone1{	"Notif.copydone1",	"The text was successfully copied to the clipboard" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct PathView {
		txt selectfile{ "PathView.selectfile",	"Select file:" };
		txt selectdir{	"PathView.selectdir",	"Select folder:" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct WinHeader {
		txt log{		"WinHeader.log",		"Log" };
		txt version{	"WinHeader.version",	"Overview" };
		txt settings{	"WinHeader.settings",	"Settings" };
		txt crtnewfile{ "WinHeader.crtnewfile",	"Creating New File" };
		txt editpu{		"WinHeader.editpu",		"Editing Pass Unit" };
		txt editfparam{ "WinHeader.editfparam",	"Editing File Settings" };
		txt copyf{		"WinHeader.copyf",		"Copy File" };
		txt pinverif{	"WinHeader.pinverif",	"PIN verification" };
	};

	// ----------------------------------------------------------------------------------------------------------------

	struct Version {
		txt h_privacy{		"Version.h_privacy",	"Privacy" };
		txt c_privacy{		"Version.c_privacy",	"Loswors is a local password storage app with open-source code, created by one person as a creative project.\n\nThe application has NOT undergone independent security audits, and its cryptographic strength has NOT been verified.\n(Storing important information in this program's save files) / (Using the application to store important data) is YOUR personal choice and is done at YOUR own risk.\n\nAuthor positions this application as an alternative to storing passwords in plain text files or other unencrypted forms.\nThe source code can be viewed via the link on GitHub." };
		
		txt h_license{		"Version.h_license",	"License" };
		txt c_license{		"Version.c_license",	"Copyright (c) 2026, HARD_VVV\n\nThe Loswors project is distributed under the (BSD 3-Clause License).\n\nThe license permits the free use, copying, modification, and distribution of the source code and the program, including in commercial and closed-source projects, provided that the terms of the license are followed. When distributing the software, copyright notices, the license text, and the disclaimer of warranties must be preserved.\n\nThe full text of the license can be found on GitHub or in the directory next to the executable file." };
		
		txt h_motiv{		"Version.h_motiv",		"Motivation" };
		txt c_motiv{		"Version.c_motiv",		"I had a need to store account passwords or important short pieces of information (for example, a bicycle lock code) in one place. At the same time, when creating a new password, a situation arises where a password that is too complex is easy to forget, while one that is easy to remember is often used for different tasks, which significantly increases the risk of being hacked.\nAlso, using a text file for storage or browser autocomplete did not give me a sense of reliability.\n\nTherefore, I wanted to create a small local application that would make it possible to store passwords in one place, as well as understand the logic behind data security.\n\nWhile writing this application, I analyzed how file encryption works in general, and what logical operations and sequences of actions are used in the industry to ensure cryptographic strength.\nI wanted to make a local application and developed my own logic and philosophy for how it works.\n\nThe project uses both proven cryptographically strong algorithms and often custom algorithms whose cryptographic strength has not been proven (this is my project, and I wanted to experiment). In any case, the code is open, and anyone can write their own version of the encoding. I think open code is a good idea because the topic is very important, and a person can see for themselves exactly what happens to their data instead of simply hoping that everything is okay." };
		
		txt h_howitwork{	"Version.h_howitwork",	"How it works" };
		txt c_howitwork{	"Version.c_howitwork",	"Loswors is a local application that uses its own save file format and stores data offline on the device.\nThe main task is to protect data against offline cracking attempts in a situation where the encoding algorithm and the save file are known. The only thing that remains unknown is the password for the file (MASTER PASS).\nMASTER PASS is the main secret known only to the user. After it is entered, a cascade of logical operations for decrypting the data takes place, and the data is displayed on the screen in its clear form.\n\nThe operating logic assumes that any MASTER PASS will always trigger file decryption, whether it is correct or not. If MASTER PASS is correct, the user will be shown the data they entered when saving. If MASTER PASS is incorrect, the application will still decode the file and show the decoding result, which will produce pseudorandom binary noise.\n\nThis is done to make identifying the correct password more difficult (although some information indicates that this approach does not significantly increase cryptographic strength) but the application operates according to this logic. No one knows whether the correct password was entered, this can only be determined if the decoding result produces readable text that the user entered when saving.\n\nThis leads to a nuance of use: the application does not prohibit adding a new entry with an incorrect password, but it will not be possible to save new data, since the entered PIN (even if it is entered correctly) will not match the hash in the file, since the password hash will not be the same as when it was initially saved.\n\nThe application operates according to a strict file-processing pipeline (detailed information about each stage is described below):\n\nIf dual-file mode is selected, the data from 2 files is first combined bit by bit into a single byte array (Binary Shuffle), where the even bits are taken from the first file and the odd bits from the second.\nNext, the byte array is checked for format compliance, the version is checked, and the file hash is checked.\nAfter the password is entered, it runs NFC UTF-8 normalization to get a byte sequence from the entered characters. The resulting byte sequence is mixed with the MASTER SALT (which is stored directly in the file in open form) using the Argon2id KDF function. The result is MASTER HASH. Next, an initial global shuffle of the entire part of the file containing secret data is based on MASTER HASH, without taking the internal structure into account (Global XOR Shuffle). Then secondary decryption takes place. The data in the secret area of the file is logically divided into fixed-length elements called PASS UNIT (using any structure other than fixed division would be an indicator of correct decryption, which I avoid). Each PASS UNIT contains 3 fields (name, password, description), as well as an INSIDE SALT for each of them. For each PASS UNIT, the following chain of operations takes place - MASTER HASH together with INSIDE SALT are mixed in my custom KDF, and the resulting INSIDE HASH is used as the key for the shuffling algorithm (Inside XOR Shuffle). This operation is repeated separately for each field, using that field's INSIDE SALT.\n\nAs a result, the data is written to UI elements, which store the data for display to the user in clear, decrypted form. The data from these elements is also used while next saving. While running, the application strictly ensures that important secret information remains in RAM for as little time as possible. Therefore, memory is overwritten with zeros everywhere secret data is handled, after the information is no longer needed." };
		
		txt c_binshuffle{	"Version.c_binshuffle",	"The data, which logically represents a single byte array (save file), is split into 2 separate files, with the even bits being written to the first file and the odd bits to the second.\n\nThe application usage mode in which one file is split into 2 files was added to increase the level of protection for particularly important data. Having only one part of the file, it is physically impossible to restore the saved data.\n\nThe intended use case is one in which the 2 files are stored on different drives, one of which is not constantly connected (for example, a USB flash drive), and physically inserting the flash drive into the device is required to access the data.\n(The application was developed for this use case, which is why there is a setting that makes the application immediately clear the data and return to the start screen when the file path disappears)" };
		txt c_filehash{		"Version.c_filehash",	"The file hash is a set of bytes at the end of the file that are used to verify as a correct file for this program. It is calculated from the data of the entire file, excluding the hash itself, and is not part of the encryption. It can be easily fake, but it is only needed to quickly determine whether the file has been corrupted and whether the program is opening the correct file at all." };
		txt c_argon2id{		"Version.c_argon2id",	"Argon2id is a KDF algorithm that allows MASTER PASS to be mixed with MASTER SALT to get MASTER HASH.\nIt is a strong, cryptographically secure algorithm and the main protector of data privacy. It was the winner of the Password Hashing Competition (PHC) in 2015.\nIn my program, it is configured to require a large amount of RAM and consume a lot of processor time, thereby making offline brute-force attacks more difficult. This is very important because offline brute-force attacks are the main threat to a local save file when the encoding algorithm is known. The requirement for a large amount of RAM reduces the advantages of GPU brute force." };
		txt c_gxor{			"Version.c_gxor",		"Global data shuffling is my custom algorithm that takes MASTER HASH as input and uses it as a key for reversible encryption of the file area containing encoded data.\nUsing deterministic noise generation, I locally expand MASTER HASH to the size of the file, then perform an XOR operation between MASTER HASH and the data from the file, and also swap bytes using MASTER HASH as the source of indices.\nThis algorithm blindly processes the entire encrypted area without taking its internal structure into account." };
		txt c_ixor{			"Version.c_ixor",		"After global shuffling, the data is logically divided into PASS UNITs, each of which stores 3 INSIDE SALT fields for the internal fields (name, password, description).\nInside XOR Shuffle is a set of operations that takes MASTER HASH and mixes it with INSIDE SALT through my custom KDF separately for each field, obtaining 3 different INSIDE HASH values. Then, a reversible lightweight algorithm similar to Global XOR Shuffle is launched. It takes the resulting INSIDE HASH and performs an XOR operation with the data in its field. Also, based on INSIDE HASH, every 2 neighboring bytes of the field are either swapped or an XOR operation is performed between them.\nIn the end, we obtain the final data, which is written to UI elements for rendering." };
	};


	// main holder -------
	struct ALL {
		StartST Sst;
		MainST Mst;

		SettingsPN settings;
		DownBarPN downbar;

		Drager drager;

		FileInfo finfo;
		PassGen passgen;
		PUBlock publock;
		Notif notif;

		PathView pathview;

		WinHeader winheader;

		Version version;
	};

	inline const ALL t{};

	// arr for Localization to fill
	inline const txt* const arr[] = {

		&t.version.h_privacy,
		&t.version.c_privacy,
		&t.version.h_license,
		&t.version.c_license,
		&t.version.h_motiv,
		&t.version.c_motiv,
		&t.version.h_howitwork,
		&t.version.c_howitwork,
		&t.version.c_binshuffle,
		&t.version.c_filehash,
		&t.version.c_argon2id,
		&t.version.c_gxor,
		&t.version.c_ixor,

		// ---

		&t.winheader.log,
		&t.winheader.version,
		&t.winheader.settings,
		&t.winheader.crtnewfile,
		&t.winheader.editpu,
		&t.winheader.editfparam,
		&t.winheader.copyf,
		&t.winheader.pinverif,
		
		// ---

		&t.pathview.selectfile,
		&t.pathview.selectdir,
		
		// ---
		
		&t.notif.core.sv.crtdone0,
		&t.notif.core.sv.crtdone1,
		&t.notif.core.sv.savedone0,
		&t.notif.core.sv.savedone1,
		&t.notif.core.sv.errdirnotexist0,
		&t.notif.core.sv.errdirnotexist1,
		&t.notif.core.sv.errfileexis0,
		&t.notif.core.sv.errfileexis1,
		&t.notif.core.sv.errsave0,
		&t.notif.core.sv.errsave1,

		&t.notif.core.inptverf.newpasserr0,
		&t.notif.core.inptverf.newpasserr1,
		&t.notif.core.inptverf.oldpasserr0,
		&t.notif.core.inptverf.oldpasserr1,
		&t.notif.core.inptverf.pinasciierr0,
		&t.notif.core.inptverf.pinasciierr1,
		&t.notif.core.inptverf.newpinasciierr0,
		&t.notif.core.inptverf.newpinasciierr1,
		&t.notif.core.inptverf.pinhasherr0,
		&t.notif.core.inptverf.pinhasherr1,

		&t.notif.core.tryentr.errfread0,
		&t.notif.core.tryentr.errfread1,
		&t.notif.core.tryentr.warnfdownver0,
		&t.notif.core.tryentr.warnfdownver1,
		&t.notif.core.tryentr.errfupver0,
		&t.notif.core.tryentr.errfupver1,
		&t.notif.core.tryentr.errfsize0,
		&t.notif.core.tryentr.errfsize1,
		&t.notif.core.tryentr.errfformat0,
		&t.notif.core.tryentr.errfformat1,
		&t.notif.core.tryentr.errargon0,
		&t.notif.core.tryentr.errargon1,

		&t.notif.core.unsavewarn0,
		&t.notif.core.unsavewarn1,
		&t.notif.core.changelost0,
		&t.notif.core.changelost1,

		&t.notif.copydone0,
		&t.notif.copydone1,

		// ---

		&t.publock.okbtn,
		&t.publock.canselbtn,
		&t.publock.delquestion,
		&t.publock.name,
		&t.publock.password,
		&t.publock.description,

		// ---

		&t.passgen.length,

		// ---

		&t.finfo.header,
		&t.finfo.version,
		&t.finfo.size,
		&t.finfo.PUcount,
		&t.finfo.opentime,
		&t.finfo.entropy,
		&t.finfo.Hdiv,
		&t.finfo.totalsec,
		&t.finfo.gxor,
		&t.finfo.ixor,

		// ---

		&t.drager.nodrop,
		&t.drager.f0,
		&t.drager.f1,
		&t.drager.onef,

		// ---

		&t.downbar.closeesc,

		// ---

		&t.settings.forgetpath,
		&t.settings.nofileexit,
		&t.settings.decodeanim,
		&t.settings.randomtheme,
		&t.settings.inacttimeout,
		&t.settings.sec,
		&t.settings.selectlang,
		&t.settings.setdefwh,

		// ---

		&t.Mst.header.search,
		&t.Mst.header.name,
		&t.Mst.header.desc,
		&t.Mst.header.savebtn,
		&t.Mst.header.fsettbtn,

		&t.Mst.PUs.notfound,
		&t.Mst.PUs.newbtn,

		&t.Mst.PINconfirm.enterpin,
		&t.Mst.PINconfirm.warning,

		&t.Mst.FparFcpy.fparambtn,
		&t.Mst.FparFcpy.fcopybtn,
		&t.Mst.FparFcpy.warning0,
		&t.Mst.FparFcpy.warning1,
		&t.Mst.FparFcpy.fname,
		&t.Mst.FparFcpy.twofile,
		&t.Mst.FparFcpy.oldz,
		&t.Mst.FparFcpy.newz,
		&t.Mst.FparFcpy.oldpin,
		&t.Mst.FparFcpy.newpin,
		&t.Mst.FparFcpy.oldpassconf,
		&t.Mst.FparFcpy.newpassenter,
		&t.Mst.FparFcpy.newpassconf,

		&t.Mst.editPU.name,
		&t.Mst.editPU.pass,
		&t.Mst.editPU.desc,
		&t.Mst.editPU.closebtn,
		&t.Mst.editPU.okbtn,

		// ---

		&t.Sst.header.nofile,
		&t.Sst.header.forgetpath,
		&t.Sst.header.fcount,
		&t.Sst.header.newbtn,
		&t.Sst.header.closebtn,

		&t.Sst.newf.name,

		&t.Sst.passz.pass,
		&t.Sst.passz.passexpln,
		&t.Sst.passz.enterpass,
		&t.Sst.passz.confirmpass
	};
	constexpr size_t size = sizeof(arr) / sizeof(arr[0]);
}