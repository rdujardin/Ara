/*
botTrajectories.h (part of Ara, https://github.com/rdujardin/Ara)

Copyright (c) 2016, Raphaël Dujardin (rdujardin) & Jean Boehm (jboehm1)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef BOT_TRAJECTORIES_H
#define BOT_TRAJECTORIES_H

#include <iostream>
#include "common.h"
#include "botState.h"

class BotTrajectories {
public:
	static void genTrajectory(Position a,Position b,Trajectory& _trajectory,TrajIt& _trajIt,double step=0.3);
	static BotState computeAngles(BotState state);

	static void initStartUpRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state);
	static void initPreShutDownRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state);
	static void initShutDownRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state);
	static bool loopStartUpRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt);
	static bool loopPreShutDownRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt);
	static bool loopShutDownRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt);
};

#endif
