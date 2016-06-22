#include "pch.h"
FBSession^ sess = FBSession::ActiveSession;
sess->LogoutAsync();
