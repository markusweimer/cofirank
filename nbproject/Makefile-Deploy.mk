#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=

# Macros
PLATFORM=GNU-MacOSX

# Include project Makefile
include CofiRank-Makefile.mk

# Object Directory
OBJECTDIR=build/Deploy/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/loss/moviephaselossfunction.o \
	${OBJECTDIR}/src/cofi/useriterator.o \
	${OBJECTDIR}/src/bmrm/solver/innersolver.o \
	${OBJECTDIR}/src/cofi/eval/normevaluator.o \
	${OBJECTDIR}/src/utils/configexception.o \
	${OBJECTDIR}/src/utils/ublastools.o \
	${OBJECTDIR}/src/loss/lap.o \
	${OBJECTDIR}/src/bmrm/solver/daifletcherpgm.o \
	${OBJECTDIR}/src/loss/lossfunctionfactory.o \
	${OBJECTDIR}/src/cofi/usertrainer.o \
	${OBJECTDIR}/src/bmrm/solver/dualinnersolver.o \
	${OBJECTDIR}/src/cofi/eval/binaryevaluator.o \
	${OBJECTDIR}/src/loss/ndcgdomainmodel.o \
	${OBJECTDIR}/src/cofi/cfbmrm-train.o \
	${OBJECTDIR}/src/cofi/problem.o \
	${OBJECTDIR}/src/cofi/eval/csvfileevaluator.o \
	${OBJECTDIR}/src/cofi/solver.o \
	${OBJECTDIR}/src/core/cofiexception.o \
	${OBJECTDIR}/src/utils/configuration.o \
	${OBJECTDIR}/src/cofi/eval/meansquarederror.o \
	${OBJECTDIR}/src/loss/graphkernellosswrapper.o \
	${OBJECTDIR}/src/utils/timer.o \
	${OBJECTDIR}/src/cofi/cofibmrm.o \
	${OBJECTDIR}/src/loss/preferencerankingdomainmodel.o \
	${OBJECTDIR}/src/bmrm/bmrm.o \
	${OBJECTDIR}/src/loss/adaptiveregularizationlosswrapper.o \
	${OBJECTDIR}/src/cofi/movietrainer.o \
	${OBJECTDIR}/src/loss/leastsquaredomainmodel.o \
	${OBJECTDIR}/src/loss/userloss.o \
	${OBJECTDIR}/src/cofi/eval/ndcgevaluator.o \
	${OBJECTDIR}/src/cofi/eval/timeevaluator.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-D NDEBUG
CXXFLAGS=-D NDEBUG

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE} -j4 -f nbproject/Makefile-Deploy.mk dist/cofirank-deploy

dist/cofirank-deploy: ${OBJECTFILES}
	${MKDIR} -p dist
	${LINK.cc} -o dist/cofirank-deploy ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/loss/moviephaselossfunction.o: src/loss/moviephaselossfunction.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/moviephaselossfunction.o src/loss/moviephaselossfunction.cpp

${OBJECTDIR}/src/cofi/useriterator.o: src/cofi/useriterator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/useriterator.o src/cofi/useriterator.cpp

${OBJECTDIR}/src/bmrm/solver/innersolver.o: src/bmrm/solver/innersolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/bmrm/solver
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/bmrm/solver/innersolver.o src/bmrm/solver/innersolver.cpp

${OBJECTDIR}/src/cofi/eval/normevaluator.o: src/cofi/eval/normevaluator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi/eval
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/eval/normevaluator.o src/cofi/eval/normevaluator.cpp

${OBJECTDIR}/src/utils/configexception.o: src/utils/configexception.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/utils
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/utils/configexception.o src/utils/configexception.cpp

${OBJECTDIR}/src/utils/ublastools.o: src/utils/ublastools.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/utils
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/utils/ublastools.o src/utils/ublastools.cpp

${OBJECTDIR}/src/loss/lap.o: src/loss/lap.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/lap.o src/loss/lap.cpp

${OBJECTDIR}/src/bmrm/solver/daifletcherpgm.o: src/bmrm/solver/daifletcherpgm.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/bmrm/solver
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/bmrm/solver/daifletcherpgm.o src/bmrm/solver/daifletcherpgm.cpp

${OBJECTDIR}/src/loss/lossfunctionfactory.o: src/loss/lossfunctionfactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/lossfunctionfactory.o src/loss/lossfunctionfactory.cpp

${OBJECTDIR}/src/cofi/usertrainer.o: src/cofi/usertrainer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/usertrainer.o src/cofi/usertrainer.cpp

${OBJECTDIR}/src/bmrm/solver/dualinnersolver.o: src/bmrm/solver/dualinnersolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/bmrm/solver
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/bmrm/solver/dualinnersolver.o src/bmrm/solver/dualinnersolver.cpp

${OBJECTDIR}/src/cofi/eval/binaryevaluator.o: src/cofi/eval/binaryevaluator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi/eval
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/eval/binaryevaluator.o src/cofi/eval/binaryevaluator.cpp

${OBJECTDIR}/src/loss/ndcgdomainmodel.o: src/loss/ndcgdomainmodel.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/ndcgdomainmodel.o src/loss/ndcgdomainmodel.cpp

${OBJECTDIR}/src/cofi/cfbmrm-train.o: src/cofi/cfbmrm-train.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/cfbmrm-train.o src/cofi/cfbmrm-train.cpp

${OBJECTDIR}/src/cofi/problem.o: src/cofi/problem.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/problem.o src/cofi/problem.cpp

${OBJECTDIR}/src/cofi/eval/csvfileevaluator.o: src/cofi/eval/csvfileevaluator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi/eval
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/eval/csvfileevaluator.o src/cofi/eval/csvfileevaluator.cpp

${OBJECTDIR}/src/cofi/solver.o: src/cofi/solver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/solver.o src/cofi/solver.cpp

${OBJECTDIR}/src/core/cofiexception.o: src/core/cofiexception.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/core
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/core/cofiexception.o src/core/cofiexception.cpp

${OBJECTDIR}/src/utils/configuration.o: src/utils/configuration.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/utils
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/utils/configuration.o src/utils/configuration.cpp

${OBJECTDIR}/src/cofi/eval/meansquarederror.o: src/cofi/eval/meansquarederror.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi/eval
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/eval/meansquarederror.o src/cofi/eval/meansquarederror.cpp

${OBJECTDIR}/src/loss/graphkernellosswrapper.o: src/loss/graphkernellosswrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/graphkernellosswrapper.o src/loss/graphkernellosswrapper.cpp

${OBJECTDIR}/src/utils/timer.o: src/utils/timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/utils
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/utils/timer.o src/utils/timer.cpp

${OBJECTDIR}/src/cofi/cofibmrm.o: src/cofi/cofibmrm.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/cofibmrm.o src/cofi/cofibmrm.cpp

${OBJECTDIR}/src/loss/preferencerankingdomainmodel.o: src/loss/preferencerankingdomainmodel.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/preferencerankingdomainmodel.o src/loss/preferencerankingdomainmodel.cpp

${OBJECTDIR}/src/bmrm/bmrm.o: src/bmrm/bmrm.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/bmrm
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/bmrm/bmrm.o src/bmrm/bmrm.cpp

${OBJECTDIR}/src/loss/adaptiveregularizationlosswrapper.o: src/loss/adaptiveregularizationlosswrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/adaptiveregularizationlosswrapper.o src/loss/adaptiveregularizationlosswrapper.cpp

${OBJECTDIR}/src/cofi/movietrainer.o: src/cofi/movietrainer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/movietrainer.o src/cofi/movietrainer.cpp

${OBJECTDIR}/src/loss/leastsquaredomainmodel.o: src/loss/leastsquaredomainmodel.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/leastsquaredomainmodel.o src/loss/leastsquaredomainmodel.cpp

${OBJECTDIR}/src/loss/userloss.o: src/loss/userloss.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/loss
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/loss/userloss.o src/loss/userloss.cpp

${OBJECTDIR}/src/cofi/eval/ndcgevaluator.o: src/cofi/eval/ndcgevaluator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi/eval
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/eval/ndcgevaluator.o src/cofi/eval/ndcgevaluator.cpp

${OBJECTDIR}/src/cofi/eval/timeevaluator.o: src/cofi/eval/timeevaluator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/cofi/eval
	$(COMPILE.cc) -g -Isrc -Ilibs -o ${OBJECTDIR}/src/cofi/eval/timeevaluator.o src/cofi/eval/timeevaluator.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Deploy
	${RM} dist/cofirank-deploy

# Subprojects
.clean-subprojects:
