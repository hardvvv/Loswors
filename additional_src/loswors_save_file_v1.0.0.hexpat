// V 1.0.0
#include <std/mem.pat>

struct PathUnit {
    u8 name_salt[32][[color("770066")]];
    u8 name[128][[color("ff00ff")]];
    
    u8 pass_salt[32][[color("007766")]];
    u8 pass[512][[color("00ffff")]];
    
    u8 desc_salt[32][[color("667700")]];
    u8 desc[512][[color("ffff00")]];
};

struct LosworsFile {
    u16 vMAIN [[color("FF0000")]];
    u16 vFILE [[color("00FF00")]];
    u16 vADD [[color("FFF000")]];
    u8 WARNING[146] [[color("ff01ff")]];
    u8 pin_salt[32] [[color("0000FF")]];
    u8 pin_hash[32] [[color("F100A1")]];
    u8 master_salt[512] [[color("FFFF00")]];
   
    PathUnit main_data[while($<std::mem::size() - 64)];
    
    u8 file_hash[64] [[color("FF0000")]];
};

LosworsFile file @ 0x00;