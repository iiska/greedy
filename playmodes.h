#ifndef _PLAYMODES_H
#define _PLAYMODES_H

#define CLASSIC 0
#define BONUSES_AND_TRAPS 1

#define GBONUS 10
#define DBONUS 11
#define TTRAP 12
#define GTRAP 13

char playmodeStr[2][20] = {"Classic","Bonuses and Traps"};

struct playmode {
	int pm_id;
	/* Number of Greedy bonuses, which doubles scores for a few moves
	 * Greedy bonus character: $
	 */
	int gbonusc;
	int gbonus_moves;
	/* Number of Diamond bonuses, which gives more scores
	 * Diamond character: *
	 */
	int dbonusc;
	int dbonus_score;
	/* Number of Teleport traps, which teleport player
	 * to random _not_ empty location
	 * Character: T
	 */
	int ttrapc;
	/* Number of Gas traps, which confuses player
	 * Direction of next moves is random.
	 * Character: G
	 */
	int gtrapc;
	int gtrap_moves;
};

struct playmode currentPm;

void setPlaymode(int newmode);
void setPlaymodeArgs(int npmid, int ngbonusc, int ngbonus_moves, int ndbonusc, int
ndbonus_score, int nttrapc, int ngtrapc, int ngtrap_moves);

#endif
