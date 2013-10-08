#ifndef INSTR_H 
#define INSTR_H

#define ARM 1
#define THUMB 2

#define ARMBRANCHIMM_NUM 1
#define ARMBRANCHREG_NUM 2
#define ARMLOADSINGAL_NUM 3
#define ARMLOADMUL_NUM 4
#define TBRANCHREG_NUM 5
#define ARMPOPMUL_NUM 6
#define ARMPOPSINGAL_NUM 7
#define ARMDATAPOCREG_NUM 8
#define ARMDATAPRCIMM_NUM 9
#define TBRANCHIMM_NUM 10
#define TCMPBRANCH_NUM 11
#define TMOVREG_NUM 12
#define TADDREG_NUM 13
#define TBRANCHMISCELL_NUM 14
#define TLOADMUL_NUM 15
#define TPOPSINAL_NUM 16
#define TTABLEBRANCH_NUM 17
#define TLOADWORD_NUM 18

#define HEX__(n) 0x##n##LU
 
/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0) \
        +((x&0x000000F0LU)?2:0) \
        +((x&0x00000F00LU)?4:0) \
        +((x&0x0000F000LU)?8:0) \
        +((x&0x000F0000LU)?16:0) \
        +((x&0x00F00000LU)?32:0) \
        +((x&0x0F000000LU)?64:0) \
        +((x&0xF0000000LU)?128:0)
 
/* *** user macros ***/
 
/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))
 
/* for upto 16-bit binary constants, MSB first */
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
            + B8(dlsb))
 
/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
                + ((unsigned long)B8(db2)<<16) \
                + ((unsigned long)B8(db3)<<8) \
                + B8(dlsb))


typedef unsigned  int BITS32;

int state;
//int IsGotoInstr(INSTR instr);
//arm

/**arm instruction:(b bl blx)immediate
 *b:cond!=1111,link=0,imm32=imm24:0
 *bl:cond!=1111,link=1,imm32=imm24:0
 *blx:cond=1111,imm32=imm24:link:0
 */
typedef struct BRANCHIMMEDIATE_st
{
	BITS32 imm24 : 24;
	BITS32 link : 1;
	BITS32 be101 : 3;
	BITS32 cond : 4;
}BRANCHIMMEDIATE;


/* 
arm inastruction:(blx bx bxj)register   
blx:cond!=1111,notbe00=11,register=rm
bx:cond!=1111,notbe=01,register=rm
bxj:cond!=1111,notbe=10,register=rm
*/
typedef struct BRANCHRES_st
{
	BITS32 rm : 4;
	BITS32 notBe00 : 2;
	BITS32 be00 : 2;
	BITS32 notUsed : 12;
	BITS32 be00010010 : 8;
	BITS32 cond : 4;
}BRANCHRES;

/* 
 load instruction                                                                    
*/
typedef struct LOADSINGLE_st
{
	BITS32 bits1 : 4;
	BITS32 b : 1;
	BITS32 bits2 : 11;
	BITS32 rn : 4;
	BITS32 be1 : 1;
	BITS32 w : 1;
	BITS32 be0 : 1;
	BITS32 u : 1;
	BITS32 p : 1;
	BITS32 a : 1;
	BITS32 be01 : 2;
	BITS32 cond : 4;
}LOADSINGLE;

/* 
LDM/LDMIA/LDMFD,LDMDA/LDMFA,LDMDB/LDMEA,LDMIB/LDMED
registers(15)=1,then ldm(DA/DB/IB) is a branch instruction                                                                     
*/
typedef struct LOADMULTIPLE_st
{
	BITS32 registers : 15;
	BITS32 rn : 4;
	BITS32 be1 : 1;
	BITS32 w : 1;
	BITS32 is1 : 1;
	BITS32 state : 2;
	BITS32 be100 : 3;
	BITS32 cond : 4;
}LOADMULTIPLE;

/* 
pop more than one register    
if bits1[15]=1,then it is a branch instruction                                             
 */
typedef struct POPMUL_st
{
	BITS32 bits : 16;
	BITS32 be100010111101 : 12;
	BITS32 cond : 4;
}POPMUL;

/* 
pop one register    
if rt=1,then it is a branch instruction                                             
 */
typedef struct POPSINGAL_st
{
	BITS32 be000000000100 : 12;
	BITS32 rt : 4;
	BITS32 be010010011101 : 12;
	BITS32 cond : 4;
}POPSINGAL;

/* 
date processing register
bits2[0..3]=15,then  date processing register is a branch instruction                                                                    */
/************************************************************************/
typedef struct DATEPROCESSINGRES_st
{
	BITS32 bits1 : 4;
	BITS32 be0 : 1;
	BITS32 op3 : 2;
	BITS32 op2 : 5;
	BITS32 bits2 : 8;
	BITS32 op1 : 5;
	BITS32 be000 : 3;
	BITS32 cond : 4;
}DATEPROCESSINGRES;

/* 
date processing immediate
bits1[12..15]=15,then date processing immdiate is a branch instruction                                                                     */
/************************************************************************/
typedef struct DATEPROCESSINGIMM_st
{
	BITS32 bits1 : 16;
	BITS32 rn : 4;
	BITS32 op : 5;
	BITS32 be001 : 3;
	BITS32 cond : 4;
}DATEPROCESSINGIMM;



//thumb 16bits

/* 
thumb 16bits b instruction
tb1(cond) not be 111x,not permitted in IT Block.                                                                   
*/
typedef struct TB1_st
{
	BITS32 imm8 : 8;
	BITS32 cond : 4;
	BITS32 be1101 : 4;
}TB1;

typedef struct TB2_st
{
	BITS32 imm11 : 11;
	BITS32 be11100 : 5;
}TB2;

typedef union TB_st
{
	TB1 tB1;
	TB2 tB2;
}TB;

/* 
 bx blx register
 blx:l==1,bx l==0;
*/
typedef struct TBRANCHRES_st
{
	BITS32 notUsed : 3;
	BITS32 rm : 4;
	BITS32 l : 1;
	BITS32 be01000111 : 8;
}TBRANCHRES;

/* 
CBNZ ,CBZ                                                                     
*/
typedef struct COMPAREBRANCH_st
{
	BITS32 rn : 4;
	BITS32 imm5 : 5;
	BITS32 be1 : 1;
	BITS32 i : 1;
	BITS32 be0 : 1;
	BITS32 op : 1;
	BITS32 be1011 : 4;
}COMPAREBRANCH;

/* 
 *mov register  instruction
 *rd=[rd2:rd1],rd=15,then it is a branch instruction                                                                    
 **/
typedef struct TMOVERES1_st
{
	BITS32 rd1 : 3;
	BITS32 rm : 4;
	BITS32 rd2 : 1;
	BITS32 be01000110 : 8;
}TMOVERES1;

typedef struct TMOVERES2_st
{
	BITS32 rd : 4;
	BITS32 rm : 4;
	BITS32 be0000000000 : 10;
}TMOVERES2;

typedef union TMOVERES_st
{
	TMOVERES1 tMoveRes1;
	TMOVERES2 tMoveRes2;
}TMOVERES;

/* 
 * ADD  register instruction      
 * rd=[rd2:rd1],rd=15,then it is a branch instruction                                                                 
 * */
typedef struct TADDRES_st
{
	BITS32 rd1 : 3;
	BITS32 rm : 4;
	BITS32 rd2 : 1;
	BITS32 be01000100 : 8;
}TADDRES;

//thumb 32 bits

/*      
 * branch and miscellaneous control instrucition
 * b:op1=0x0,op[4:5]!=111 or op1=0x1
 * blx:op1=1x0
 * bl:op1=1x1
 * bxj :op1=0x0,op=0111100   
 * subs pc,lr:op1=0x0,op=0111101                                                          
 */
typedef struct T2BRANCHANDMISCONTROL_st
{
	BITS32 bits1 : 8;
	BITS32 op2 : 4;
	BITS32 op1 : 3;
	BITS32 be1 : 1;
	BITS32 bits2 : 4;
	BITS32 op : 7;
	BITS32 be11110 : 5;
}TBRANCHANDMISCONTROL;


/* load multiple and pop instruction
 * if bits[15]=15 ,it is a branch instruction
 * */
typedef struct T2LOADMULTIPLE_st
{
	BITS32 bits1 : 16;
	BITS32 rn : 4;
	BITS32 be1 : 1;
	BITS32 bits2 : 1;
	BITS32 be1110100010 : 10;
}TLOADMULTIPLE;


/* load word
 *if bits2=15,then it is a branch instruction                           
 */
typedef struct T2LOADWORD_st
{
	BITS32 bits1 : 6;
	BITS32 op2 : 6;
	BITS32 bits2 : 4;
	BITS32 rn : 4;
	BITS32 be101 : 3;
	BITS32 op1 : 2;
	BITS32 be1111100 : 7;
}TLOADWORD;

typedef struct T2TABLEBRANCH_st
{
	BITS32 rm : 4;
	BITS32 h : 1;
	BITS32 be000 : 3;
	BITS32 notUsed : 8;
	BITS32 rn : 4;
	BITS32 be111010001101 : 12;
}TTABLEBRANCH;

/* 
pop contains one register
rt=15,it is a branch instruction                                                                     
*/
typedef struct T2POPSINGAL_st
{
	BITS32 be101100000100 : 12;
	BITS32 rt : 4;
	BITS32 be1111100001011101 : 16;
}TPOPSINGAL;


//instruction

typedef union INSTR_st
{
	//arm
	BRANCHIMMEDIATE branchImmediate;
	BRANCHRES branchGes;
	LOADSINGLE loadSingle;
	LOADMULTIPLE loadMultiple;
	POPMUL popMul;
	POPSINGAL popSingal;
	DATEPROCESSINGRES dataProcssingRes;
	DATEPROCESSINGIMM dataProcssingImm;
		
	//thumb 16
	TB tb;
	TBRANCHRES tbranchRes;
	COMPAREBRANCH compareBranch;
	TMOVERES tmoveRes;
	TADDRES taddRes;
		
	//thumb 32
	TBRANCHANDMISCONTROL tbranchAndMis;
	TLOADWORD tloadWord;
	TLOADMULTIPLE tloadMultiple;	
	TTABLEBRANCH ttableBranch;
	TPOPSINGAL tpopSingal;

	BITS32 instr;
}INSTR;
#endif
