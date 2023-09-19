rule Win32_Ransomware_MZP : tc_detection malicious
{
    meta:

        author              = "ReversingLabs"

        source              = "ReversingLabs"
        status              = "RELEASED"
        sharing             = "TLP:WHITE"
        category            = "MALWARE"
        malware             = "MZP"
        description         = "Yara rule that detects MZP ransomware."

        tc_detection_type   = "Ransomware"
        tc_detection_name   = "MZP"
        tc_detection_factor = 5

    strings:

        $show_ransom_note_p1 = {
            55 8B EC B9 ?? ?? ?? ?? 6A ?? 6A ?? 49 75 ?? 53 56 84 D2 74 ?? 83 C4 ?? E8 ?? ?? ?? 
            ?? 88 55 ?? 8B D8 33 C0 55 68 ?? ?? ?? ?? 64 FF 30 64 89 20 33 D2 8B C3 E8 ?? ?? ?? 
            ?? 89 1D ?? ?? ?? ?? 33 C9 B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 89 73 ?? 33 C0 
            89 46 ?? 33 C0 89 86 ?? ?? ?? ?? 33 C0 89 86 ?? ?? ?? ?? C7 86 ?? ?? ?? ?? ?? ?? ?? 
            ?? C7 86 ?? ?? ?? ?? ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? C7 86 ?? ?? ?? ?? ?? ?? ?? ?? 
            6A ?? 6A ?? 8D 45 ?? 50 33 C9 B2 ?? B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 55 ?? 8D 86 ?? 
            ?? ?? ?? 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? 
            C6 86 ?? ?? ?? ?? ?? 33 C0 89 86 ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? 33 C0 89 86 ?? ?? 
            ?? ?? C6 86 ?? ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? 8D 86 ?? ?? ?? ?? 33 D2 E8 ?? ?? ?? 
            ?? 8D 86 ?? ?? ?? ?? 33 D2 E8 ?? ?? ?? ?? 33 C0 89 86 ?? ?? ?? ?? 33 C0 89 86 ?? ?? 
            ?? ?? C6 86 ?? ?? ?? ?? ?? 8D 45 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 55 ?? 8B C6 E8 
            ?? ?? ?? ?? B2 ?? 8B C6 E8 ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? 68 ?? ?? ?? ?? A1 ?? ?? 
            ?? ?? 8B 00 50 E8 ?? ?? ?? ?? 8B D0 8B C6 E8 ?? ?? ?? ?? 33 D2 8B C6 E8
        }

        $show_ransom_note_p2 = {
            C6 86 ?? ?? ?? ?? ?? C7 86 ?? ?? ?? ?? ?? ?? ?? ?? 8D 86 ?? ?? ?? ?? 33 D2 E8 ?? ?? 
            ?? ?? C6 86 ?? ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? 89 B6 ?? ?? ?? ?? C7 86 ?? ?? ?? ?? 
            ?? ?? ?? ?? 33 C0 89 86 ?? ?? ?? ?? 33 C0 89 86 ?? ?? ?? ?? 8B C6 8B 10 FF 52 ?? B2 
            ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 89 73 ?? 8D 46 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? 
            ?? C6 46 ?? ?? C6 46 ?? ?? B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 89 73 ?? C6 46 
            ?? ?? 8D 46 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 
            F0 89 73 ?? BA ?? ?? ?? ?? 8B C6 E8 ?? ?? ?? ?? B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B D0 
            8D 46 ?? E8 ?? ?? ?? ?? B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 89 73 ?? 8B C6 C6 
            40 ?? ?? 66 BA ?? ?? E8 ?? ?? ?? ?? B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F0 89 73 
            ?? 8B C6 C6 40 ?? ?? 66 BA ?? ?? E8 ?? ?? ?? ?? B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 
            89 43 ?? 8B 73 ?? 8D 46 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 46 ?? 33 D2 E8 ?? ?? ?? 
            ?? C6 46 ?? ?? C6 46 ?? ?? C6 46 ?? ?? C6 46 ?? ?? C6 46 ?? ?? 8D 46 ?? BA ?? ?? ?? 
            ?? E8 ?? ?? ?? ?? B2 ?? A1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 89 43 ?? B2 ?? A1 ?? ?? ?? ?? 
            E8 ?? ?? ?? ?? 89 43 ?? 8B 73 ?? 8D 46 ?? 33 D2 E8 ?? ?? ?? ?? 8D 46 ?? BA ?? ?? ?? 
            ?? E8
        }

        $search_config_file = {
            8B C0 53 56 8B F0 8A 9E ?? ?? ?? ?? C6 86 ?? ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? 75 ?? 
            8B 46 ?? 8B 48 ?? A1 ?? ?? ?? ?? 33 D2 E8 ?? ?? ?? ?? 8B C6 E8 ?? ?? ?? ?? 33 D2 8B 
            86 ?? ?? ?? ?? FF 96 ?? ?? ?? ?? 83 BE ?? ?? ?? ?? ?? 74 ?? 8B 96 ?? ?? ?? ?? B8 ?? 
            ?? ?? ?? E8 ?? ?? ?? ?? 85 C0 75 ?? 89 B6 ?? ?? ?? ?? C7 86 ?? ?? ?? ?? ?? ?? ?? ?? 
            EB ?? 89 B6 ?? ?? ?? ?? C7 86 ?? ?? ?? ?? ?? ?? ?? ?? 88 9E ?? ?? ?? ?? 8A 96 ?? ?? 
            ?? ?? 8B C6 E8 ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? 74 ?? 8B 46 ?? 8B 8E ?? ?? ?? ?? 8B 
            96 ?? ?? ?? ?? E8 ?? ?? ?? ?? 56 68 ?? ?? ?? ?? 8B 46 ?? E8 ?? ?? ?? ?? 8B 46 ?? 89 
            70 ?? 5E 5B C3
        }

        $track_mouse_event_for_entropy = {
            53 56 83 C4 ?? 8B F0 8B 42 ?? 05 ?? ?? ?? ?? 83 E8 ?? 72 ?? 2D ?? ?? ?? ?? 0F 84 ?? 
            ?? ?? ?? E9 ?? ?? ?? ?? 8A 86 ?? ?? ?? ?? 88 44 24 ?? 66 83 BE ?? ?? ?? ?? ?? 74 ?? 
            8B D6 8B 86 ?? ?? ?? ?? FF 96 ?? ?? ?? ?? 8B D8 EB ?? 54 E8 ?? ?? ?? ?? 8D 4C 24 ?? 
            8B D4 8B C6 E8 ?? ?? ?? ?? 8B 44 24 ?? 89 04 24 8B 44 24 ?? 89 44 24 ?? 8D 54 24 ?? 
            8B C6 E8 ?? ?? ?? ?? 8D 54 24 ?? 8B C4 E8 ?? ?? ?? ?? 8B D8 3A 5C 24 ?? 0F 84 ?? ?? 
            ?? ?? 8B C6 E8 ?? ?? ?? ?? 84 DB 74 ?? C6 86 ?? ?? ?? ?? ?? 66 83 BE ?? ?? ?? ?? ?? 
            74 ?? 8B D6 8B 86 ?? ?? ?? ?? FF 96 ?? ?? ?? ?? C7 44 24 ?? ?? ?? ?? ?? C7 44 24 ?? 
            ?? ?? ?? ?? 8B 46 ?? 89 44 24 ?? 8D 44 24 ?? E8 ?? ?? ?? ?? 8B C6 E8 ?? ?? ?? ?? EB 
            ?? C6 86 ?? ?? ?? ?? ?? C7 44 24 ?? ?? ?? ?? ?? C7 44 24 ?? ?? ?? ?? ?? 8B 46 ?? 89 
            44 24 ?? 8D 44 24 ?? E8 ?? ?? ?? ?? 66 83 BE ?? ?? ?? ?? ?? 74 ?? 8B D6 8B 86 ?? ?? 
            ?? ?? FF 96 ?? ?? ?? ?? 8B C6 E8 ?? ?? ?? ?? EB ?? 80 BE ?? ?? ?? ?? ?? 74 ?? C6 86 
            ?? ?? ?? ?? ?? 66 83 BE ?? ?? ?? ?? ?? 74 ?? 8B D6 8B 86 ?? ?? ?? ?? FF 96 ?? ?? ?? 
            ?? 8B C6 E8 ?? ?? ?? ?? 33 C0 83 C4 ?? 5E 5B C3
        }

        $find_files_p1 = {
            55 8B EC 81 C4 ?? ?? ?? ?? 53 56 57 33 C9 89 8D ?? ?? ?? ?? 89 8D ?? ?? ?? ?? 8B FA 
            8B D8 33 C0 55 68 ?? ?? ?? ?? 64 FF 30 64 89 20 8D 85 ?? ?? ?? ?? 50 8D 85 ?? ?? ?? 
            ?? B9 ?? ?? ?? ?? 8B D7 E8 ?? ?? ?? ?? 8B 85 ?? ?? ?? ?? E8 ?? ?? ?? ?? 50 E8 ?? ?? 
            ?? ?? 8B F0 83 FE ?? 0F 84 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? 89 43 ?? 8D 85 ?? ?? ?? ?? 
            8D 95 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? BA ?? ?? ?? ?? 33 C9 8A 08 41 E8 
            ?? ?? ?? ?? 0F 84 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 
            85 ?? ?? ?? ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 0F 84 ?? ?? ?? ?? F6 85 ?? ?? ?? ?? ?? 
            0F 84 ?? ?? ?? ?? 80 7B ?? ?? 76 ?? 8D 85 ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? B9 ?? ?? ?? 
            ?? E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? 8B D7 8B C3 E8 ?? ?? ?? ?? 80 7B ?? ?? 0F 85 ?? 
            ?? ?? ?? 57 8D 85 ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? FF B5 
            ?? ?? ?? ?? 68 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 95 ?? 
            ?? ?? ?? 8B C3 E8 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? 89 43 ?? E9 ?? ?? ?? ?? 8D 85 ?? ?? 
            ?? ?? 8D 95 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 85 ?? ?? ?? ?? 8D 95 ?? ?? 
            ?? ?? E8 ?? ?? ?? ?? 8B 85 ?? ?? ?? ?? 8B 53 ?? E8 ?? ?? ?? ?? 84 C0 74 ?? FF 43
        }

        $find_files_p2 = {
            80 7B ?? ?? 0F 84 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 
            ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? 8B D7 8B C3 E8 ?? ?? ?? ?? EB ?? 80 7B ?? ?? 74 ?? 8D 
            85 ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 8D ?? ?? ?? ?? 8D 
            85 ?? ?? ?? ?? 8B D7 E8 ?? ?? ?? ?? 8B 95 ?? ?? ?? ?? 8D 43 ?? E8 ?? ?? ?? ?? EB ?? 
            8D 85 ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 95 ?? ?? ?? ?? 
            8D 43 ?? E8 ?? ?? ?? ?? 80 7B ?? ?? 75 ?? 8D 85 ?? ?? ?? ?? 50 56 E8 ?? ?? ?? ?? 85 
            C0 0F 85 ?? ?? ?? ?? 56 E8 ?? ?? ?? ?? 33 C0 5A 59 59 64 89 10 68 ?? ?? ?? ?? 8D 85 
            ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? E8 ?? ?? ?? ?? C3
        }

        $encrypt_files = {
            8B C0 33 D2 89 50 ?? 89 50 ?? 52 8D 50 ?? 52 FF 70 ?? FF 70 ?? FF 30 E8 ?? ?? ?? ?? 
            85 C0 74 ?? 33 C0 C3 E8 ?? ?? ?? ?? 83 F8 ?? 74 ?? C3 33 C0 C3 51 8B 50 ?? 85 D2 7E 
            ?? 33 C9 89 48 ?? 51 8D 4C 24 ?? 51 52 FF 70 ?? FF 30 E8 ?? ?? ?? ?? 85 C0 74 ?? 33 
            C0 59 C3 E8 ?? ?? ?? ?? EB ?? FF 30 C7 40 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 75 ?? C3 
            E8 ?? ?? ?? ?? C3 56 8B F0 33 C0 89 46 ?? 89 46 ?? 8B 46 ?? 2D ?? ?? ?? ?? 74 ?? 48 
            74 ?? 48 74 ?? E9 ?? ?? ?? ?? B8 ?? ?? ?? ?? BA ?? ?? ?? ?? B9 ?? ?? ?? ?? C7 46 ?? 
            ?? ?? ?? ?? EB ?? B8 ?? ?? ?? ?? BA ?? ?? ?? ?? B9 ?? ?? ?? ?? EB ?? B8 ?? ?? ?? ?? 
            BA ?? ?? ?? ?? B9 ?? ?? ?? ?? C7 46 ?? ?? ?? ?? ?? C7 46 ?? ?? ?? ?? ?? C7 46 ?? ?? 
            ?? ?? ?? 80 7E ?? ?? 0F 84 ?? ?? ?? ?? 6A ?? 68 ?? ?? ?? ?? 51 6A ?? 52 50 8D 46 ?? 
            50 E8 ?? ?? ?? ?? 83 F8 ?? 0F 84 ?? ?? ?? ?? 89 06 81 7E ?? ?? ?? ?? ?? 0F 85 ?? ?? 
            ?? ?? FF 4E ?? 6A ?? FF 36 E8 ?? ?? ?? ?? 40 0F 84 ?? ?? ?? ?? 2D ?? ?? ?? ?? 73 ?? 
            33 C0 6A ?? 6A ?? 50 FF 36 E8 ?? ?? ?? ?? 40 0F 84 ?? ?? ?? ?? 6A ?? 8B D4 6A ?? 52 
            68 ?? ?? ?? ?? 8D 96 ?? ?? ?? ?? 52 FF 36 E8 ?? ?? ?? ?? 5A 48 0F 85 ?? ?? ?? ?? 33 
            C0 3B C2 73 ?? 80 BC 06 ?? ?? ?? ?? ?? 74 ?? 40 EB ?? 6A ?? 6A ?? 2B C2 50 FF 36 E8 
            ?? ?? ?? ?? 40 74 ?? FF 36 E8 ?? ?? ?? ?? 48 75 ?? EB ?? C7 46 ?? ?? ?? ?? ?? 81 7E 
            ?? ?? ?? ?? ?? 74 ?? 6A ?? EB ?? 6A ?? E8 ?? ?? ?? ?? 83 F8 ?? 74 ?? 89 06 81 7E ?? 
            ?? ?? ?? ?? 74 ?? FF 36 E8 ?? ?? ?? ?? 85 C0 74 ?? 83 F8 ?? 75 ?? C7 46 ?? ?? ?? ?? 
            ?? 33 C0 5E C3
        }

    condition:
        uint16(0) == 0x5A4D and
        (
            $search_config_file
        ) and
        (
            all of ($find_files_p*)
        ) and
        (
            $track_mouse_event_for_entropy
        ) and
        (
            $encrypt_files
        ) and
        (
            all of ($show_ransom_note_p*)
        )
}