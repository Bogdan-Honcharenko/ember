#include "alu.hpp"
namespace { void zn(Byte x, Flags& f) { f.z=x==0; f.n=(x&0x80U)!=0; } }
Byte alu_add(Byte a,Byte b,Flags& f){unsigned x=static_cast<unsigned>(a)+b; Byte r=static_cast<Byte>(x);zn(r,f);f.c=x>255U;return r;}
Byte alu_sub(Byte a,Byte b,Flags& f){Byte r=static_cast<Byte>(a-b);zn(r,f);f.c=a<b;return r;}
Byte alu_and(Byte a,Byte b,Flags& f){Byte r=static_cast<Byte>(a&b);zn(r,f);f.c=false;return r;}
Byte alu_or(Byte a,Byte b,Flags& f){Byte r=static_cast<Byte>(a|b);zn(r,f);f.c=false;return r;}
Byte alu_xor(Byte a,Byte b,Flags& f){Byte r=static_cast<Byte>(a^b);zn(r,f);f.c=false;return r;}
Byte alu_not(Byte a,Flags& f){Byte r=static_cast<Byte>(~a);zn(r,f);f.c=false;return r;}
Byte alu_shl(Byte a,Flags& f){f.c=(a&0x80U)!=0;Byte r=static_cast<Byte>(static_cast<unsigned>(a)<<1U);zn(r,f);return r;}
Byte alu_shr(Byte a,Flags& f){f.c=(a&1U)!=0;Byte r=static_cast<Byte>(a>>1U);zn(r,f);return r;}
Byte alu_inc(Byte a,Flags& f){Byte r=static_cast<Byte>(a+1U);zn(r,f);return r;}
Byte alu_dec(Byte a,Flags& f){Byte r=static_cast<Byte>(a-1U);zn(r,f);return r;}
