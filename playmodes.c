#include "playmodes.h"

void setPlaymode(int newmode) {
	if (newmode == CLASSIC) {
		setPlaymodeArgs(CLASSIC,0,0,0,0,0,0,0);
	}
	else if (newmode == BONUSES_AND_TRAPS) {
		// setPlaymodeArgs(BONUSES_AND_TRAPS,7,5,10,18,5,3,4);
		/* TODO: Add teleport traps, when they work */
		setPlaymodeArgs(BONUSES_AND_TRAPS,14,5,20,18,0,16,4);
	}
	else
		setPlaymodeArgs(CLASSIC,0,0,0,0,0,0,0);
}

void setPlaymodeArgs(int npmid, int ngbonusc, int ngbonus_moves, int ndbonusc, int
ndbonus_score, int nttrapc, int ngtrapc, int ngtrap_moves) {
	currentPm.pm_id = npmid;
	currentPm.gbonusc = ngbonusc;
	currentPm.gbonus_moves = ngbonus_moves;
	currentPm.dbonusc = ndbonusc;
	currentPm.dbonus_score = ndbonus_score;
	currentPm.ttrapc = nttrapc;
	currentPm.gtrapc = ngtrapc;
	currentPm.gtrap_moves = ngtrap_moves;
}
