#include "instr.h"
#include <stdio.h>

int IsArmBranchReg(INSTR instr)
{
	if(B8(00)==instr.branchGes.be00  
		&& B8(00010010)==instr.branchGes.be00010010 
		&& B8(00)!=instr.branchGes.notBe00 
		&& B8(1111)!=instr.branchGes.cond)
	{
		return 0;
	}
	return -1;
}

int IsArmBranchImm(INSTR instr)
{
	if(B8(101)==instr.branchImmediate.be101)
	{
		return 0;
	}
	return -1;
}

int IsArmLoadSinale(INSTR instr)
{
	if(B8(1)==instr.loadSingle.be1 
			&& B8(0)==instr.loadSingle.be0 
			&& B8(01)==instr.loadSingle.be01)
	{
		if(15==instr.loadSingle.rn) //rn:pc
		return 0;
	}
	return -1;
}

int IsArmLoadMul(INSTR instr)
{
	if(B8(1)==instr.loadMultiple.be1 
			&& B8(1)==instr.loadMultiple.is1 
			&& B8(100)==instr.loadMultiple.be100)
	{
		if(B8(1)==instr.loadMultiple.registers&B16(1000000,00000000))
		{
			return 0;
		}
	}
	return -1;
}

int IsArmPopMul(INSTR instr)
{
	if(B16(1000,10111101)==instr.popMul.be100010111101)
	{
		if(B8(1)==instr.popMul.bits&B16(1000000,00000000))
			return 0;
	}
	return -1;
}

int IsArmPopSingal(INSTR instr)
{
	if(B16(0000,00000100)==instr.popSingal.be000000000100
			&&	B16(0100,10011101)==instr.popSingal.be010010011101)
	{
		if(B8(1)==instr.popSingal.rt)
			return 0;
	}
	return -1;
}

int IsArmDataPocReg(INSTR instr)
{
	if(B8(0)==instr.dataProcssingRes.be0 
			&& B8(000)==instr.dataProcssingRes.be000)
	{
		if(15==instr.dataProcssingRes.bits1)
			return 0;
	}
	return -1;
}

int IsArmDataPrcImm(INSTR instr)
{
	if(B8(001)==instr.dataProcssingImm.be001)
	{
		if(15==instr.dataProcssingImm.bits1&B16(11110000,00000000))
			return 0;
	}
	return -1;
}

int IsThumbBranchImme(INSTR instr)
{
	if(B8(11100)==instr.tb.tB2.be11100 || (B8(1101)==instr.tb.tB1.be1101 && B8(1111)!=instr.tb.tB1.cond 
				&& B8(1110)!=instr.tb.tB1.cond) )
		return 0;
	return -1;
}

int IsThumbBranchReg(INSTR instr)
{
	if(B8(01000111)==instr.tbranchRes.be01000111)	
	{
		return 0;
	}
	return -1;
}

int IsThumbCmpBranch(INSTR instr)
{
	if(B8(1)==instr.compareBranch.be1 
			&& B8(0)==instr.compareBranch.be0 
			&& B8(1011)==instr.compareBranch.be1011)
	{
		return 0;
	}
	return -1;
}

int IsThumbMovReg(INSTR instr)
{
	if(B8(01000110)==instr.tmoveRes.tMoveRes1.be01000110)
	{
		if(15==(instr.tmoveRes.tMoveRes1.rd1+instr.tmoveRes.tMoveRes1.rd2*8))
			return 0;
	}
	else if(B16(00,00000000)==instr.tmoveRes.tMoveRes2.be0000000000)
	{
		if(15==instr.tmoveRes.tMoveRes2.rd)
			return 0;
	}
	return -1;
}

int IsThumbAddReg(INSTR instr)
{
	if(B8(01000100)==instr.taddRes.be01000100)
	{
		if(15==(instr.taddRes.rd1+instr.taddRes.rd2*8))
			return 0;
	}
	return -1;
}

int IsThumbBranchMiscell(INSTR instr)
{
	if(B8(1)==instr.tbranchAndMis.be1 && B8(11110)==instr.tbranchAndMis.be11110)
	{
		if((B8(001)==instr.tbranchAndMis.op1&101) ||	(B8(000)==instr.tbranchAndMis.op1&101))
				return 0;
	}
	return -1;
}

int IsThumbLoadWord(INSTR instr)
{
	if(B8(101)==instr.tloadWord.be101 && B8(1111100)==instr.tloadWord.be1111100)
	{
		if(15==instr.tloadWord.bits2)
			return 0;
	}
	return -1;
}


int IsThumbLoadMul(INSTR instr)
{
	if(B16(11,10100010)==instr.tloadMultiple.be1110100010)
	{
		if(1==instr.tloadMultiple.bits1&B16(1000000,00000000))
			return 0;
	}
	return -1;
}

int IsThumbTableBranch(INSTR instr)
{
	if(B8(000)==instr.ttableBranch.be000 
			&& B16(1110,10001101)==instr.ttableBranch.be111010001101)
		return 0;
	return -1;
}

int IsThumbPopSinal(INSTR instr)
{
	if(B16(1011,00000100)==instr.tpopSingal.be101100000100 && 
			B16(11111000,01011101)==instr.tpopSingal.be1111100001011101)
	{
		if(15==instr.tpopSingal.rt)
			return 0;
	}
	return -1;
}
void SetState(int newState)
{
	state=newState;
}

int CurentState()
{
	return state;
}


int IsGotoInstr(INSTR instr)
{
		/*b.bl.blx immediate*/
		if(0==IsArmBranchImm(instr))
			return ARMBRANCHIMM_NUM;
		else if(0==IsArmBranchReg(instr))
			return ARMBRANCHREG_NUM;
		else if(0==IsArmLoadSinale(instr))
			return ARMLOADSINGAL_NUM;
		else if(0==IsArmLoadMul(instr))
			return ARMLOADMUL_NUM;
		else if(0==IsArmPopMul(instr))
			return ARMPOPMUL_NUM;
		else if(0==IsArmPopSingal(instr))
			return ARMPOPSINGAL_NUM;
		else if(0==IsArmDataPocReg(instr))
			return ARMDATAPOCREG_NUM;
		else if(0==IsArmDataPrcImm(instr))
			return ARMDATAPRCIMM_NUM;
		else if(0==IsThumbBranchImme(instr))
			return TBRANCHIMM_NUM;
		else if(0==IsThumbBranchReg(instr))
			return TBRANCHREG_NUM;
		else if(0==IsThumbCmpBranch(instr))
			return TCMPBRANCH_NUM;
		else if(0==IsThumbMovReg(instr))
			return TMOVREG_NUM;
		else if(0==IsThumbAddReg(instr))
			return TADDREG_NUM;
		else if(0==IsThumbBranchMiscell(instr))
			return TBRANCHMISCELL_NUM;
		else if(0==IsThumbLoadWord(instr))
			return TLOADWORD_NUM;
		else if(0==IsThumbLoadMul(instr))
			return TLOADMUL_NUM;
		else if(0==IsThumbTableBranch(instr))
			return TTABLEBRANCH_NUM;
		else if(0==IsThumbPopSinal(instr))
			return TPOPSINAL_NUM;
		return -1;
}

int main()
{
#if 1
	int ret;
	INSTR tempInstr;
	tempInstr.instr=0xe59af004;
	ret=IsGotoInstr(tempInstr);
	if(ret!=-1)
		printf("is goto instr\n");
#endif
}
