#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "asciis.h"

#define CLEAR "\033[2J\033[1;1H"
#define MIN_TEAMS 4
#define MAX_TEAMS 100
#define MAX_PLAYERS 800
#define SQUAD_SIZE 8
#define PLAYING_SQUAD 5
#define LEAGUE_WEEKS 6
#define LEAGUE_TEAMS 4
#define INITIAL_BUDGET 100
#define WIN_BUDGET 5
#define DRAW_BUDGET 3
#define LOSE_BUDGET 1
#define FATIGUE_REDUCTION 5

struct TeamManager {
    char email[30];
    char password[30];
    int teamId;
};

struct FederasionManager {
    char username[30];
    char password[30];
};

struct Player {
    int id;
    char name[30];
    int attackPower;
    int defensePower;
    int value;
    char teamName[30];
};

struct Team {
    char name[30];
    char managerEmail[30];
    int teamId;
    int budget;
    struct Player teamPlayers[SQUAD_SIZE];
    int teamPlayerCounter;
    int isReadyToPlay;
    int wins;
    int loses;
    int draws;
    int gf;
    int ga;
    int gd;
    int pts;
};

struct League {
    struct Team teams[LEAGUE_TEAMS];
    struct Team teamsOpponents[LEAGUE_TEAMS][LEAGUE_WEEKS];
    int matchCounter[LEAGUE_WEEKS];
    int matchCounterPreview[LEAGUE_WEEKS];
};

struct LeagueOtherInfo {
    int currentWeek;
    int doesLeagueStarted;
    int doesTransferWindowOpened;
};

// Global variables
struct FederasionManager federasionManager;
struct LeagueOtherInfo leagueOtherInfo;
struct Team teams[MAX_TEAMS];
struct TeamManager teamManagers[MAX_TEAMS];
struct League league;
int teamsCounter = 0;
struct Player players[MAX_PLAYERS];
int playersCounter = 0;
int currentWeek = 1;
int doesLeagueStarted = 0;
int doesTransferWindowOpened = 0;

// Function declarations
void login();
void forgetPassword();
void federasionManagerMenu();
void teamManagerMenu(int index);
int doesTeamManagerExist(char username[], char password[]);
int doesTeamExist(char teamName[]);
int doesTeamManagerEmailExist(char email[]);
void addTeam();
int createIdForTeam();
int createIdForPlayer();
void changePassword(int teamIndex);
void addPlayer();
void showTeams();
void showTeam(struct Team team);
void showPlayers();
void showPlayer(struct Player player);
int getPlayerAttack(struct Player player);
int getPlayerDefense(struct Player player);
void startLeague();
void chooseTeamsForLeague();
void createFixtures();
void playWeekGames();
void startWeekGames();
int getTeamAttack(struct Team team);
int getTeamDefense(struct Team team);
void showStandings();
void openCloseTransferWindows();
void announceChampion();
void sortPlayers();
void buyPlayer(int index);
void sellPlayer(int index);
void selectSquad(int index);
void leagueStanding();
void showFixtures(int index);
void showNextOpponent(int index);
void updatePassword(int index);
void readFromFiles();
void writeToFiles();
void welcome();
void showFutureGames(int week);
void setupFixtures();
void sortTeams(struct Team *teams);
void endScreen();
void swapLeagueTeams(struct Team *teams, int i, int j);

int main() {
    printf(CLEAR);
    welcome();
    readFromFiles();
    
    strcpy(federasionManager.username, "admin");
    strcpy(federasionManager.password, "admin");
    
    while (1) {
        printf(FCYAN "1. Login\n2. Forget Password\n3. Exit\n");
        int choice;
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                forgetPassword();
                break;
            case 3:
                writeToFiles();
                exit(0);
            default:
                printf(FRED "Invalid option!\n");
        }
    }
}

void endScreen() {
    printf(FRED "\n\n\n\nOOOOOOOOOOOOO   OO        OO   OOOOOOOOO     OOOOOOOOOOO    NN         N   DDD\n");
    printf("      OO        OO        OO   OO            O              N  N       N   D   D\n");
    printf("      OO        OO        OO   OO            O              N    N     N   D    D\n");
    printf("      OO        OO        OO   OO            O              N     N    N   D     D\n");
    printf("      OO        OOOOOOOOOOOO   OOOOOOOOO     OOOOOOOOOOO    N       N  N   D     D\n");
    printf("      OO        OO        OO   OO            O              N        N N   D    D\n");
    printf("      OO        OO        OO   OO            O              N         NN   D   D\n");
    printf("      OO        OO        OO   OOOOOOOOO     OOOOOOOOOOO    N         NN   DDD\n\n\n");
}

void login() {
    char password[30];
    char username[30];
    
    printf(FYELLOW "Enter username: ");
    scanf("%s", username);
    printf(FYELLOW "Enter password: ");
    scanf("%s", password);
    
    if (strcmp(username, federasionManager.username) == 0 && 
        strcmp(password, federasionManager.password) == 0) {
        federasionManagerMenu();
    } else {
        int teamMangerIndex = doesTeamManagerExist(username, password);
        if (teamMangerIndex == -1) {
            printf(FRED "Invalid username or password!\n");
        } else {
            teamManagerMenu(teamMangerIndex);
        }
    }
}

void forgetPassword() {
    char teamName[30];
    char managerEmail[30];
    
    printf(FMAGNETA "Enter team name: ");
    scanf("%s", teamName);
    printf(FMAGNETA "Enter team manager email: ");
    scanf("%s", managerEmail);
    
    int teamIndex = doesTeamExist(teamName);
    if (teamIndex == -1 || strcmp(teams[teamIndex].managerEmail, managerEmail) != 0) {
        printf(FRED "Invalid team name or manager email!\n");
        return;
    }
    
    changePassword(teamIndex);
}

void federasionManagerMenu() {
    int running = 1;
    while (running) {
        printf(FLIMOIE "1. Add Team\n2. Add Player\n3. Show Teams\n4. Show Players\n");
        printf("5. Start League\n6. Open/Close Transfer Window\n7. Start Week %d\n", currentWeek);
        printf("8. Announce Champion and End League\n9. Exit\n");
        
        int choice;
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: addTeam(); break;
            case 2: addPlayer(); break;
            case 3: showTeams(); break;
            case 4: showPlayers(); break;
            case 5: startLeague(); break;
            case 6: openCloseTransferWindows(); break;
            case 7: playWeekGames(); break;
            case 8: announceChampion(); break;
            case 9: running = 0; break;
            default: printf(FRED "Invalid option!\n");
        }
    }
}

void welcome() {
    printf(FLIMOIE "\n\nH   H    EEEEE   L       L        OOOOO\n");
    printf("H   H    E       L       L       O     O\n");
    printf("H   H    E       L       L       O     O\n");
    printf("HHHHH    EEEEE   L       L       O     O\n");
    printf("H   H    E       L       L       O     O\n");
    printf("H   H    E       L       L       O     O\n");
    printf("H   H    EEEEE   LLLLL   LLLLL    OOOOO\n\n");
}

void teamManagerMenu(int index) {
    system("cls");
    
    int running = 1;
    while (running) {
        printf(FBLUE "1. Buy Player\n2. Sell Player\n3. Select/Submit Squad\n4. League Standing\n");
        printf("5. Fixtures\n6. Upcoming Opponent\n7. Change Password\n8. Exit\n");
        
        int choice;
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: buyPlayer(index); break;
            case 2: sellPlayer(index); break;
            case 3: selectSquad(index); break;
            case 4: leagueStanding(); break;
            case 5: showFixtures(index); break;
            case 6: showNextOpponent(index); break;
            case 7: updatePassword(index); break;
            case 8: running = 0; break;
            default: printf(FRED "Invalid option!\n");
        }
    }
}

int doesTeamManagerExist(char username[], char password[]) {
    for (int i = 0; i < teamsCounter; i++) {
        if (strcmp(username, teamManagers[i].email) == 0 && 
            strcmp(password, teamManagers[i].password) == 0) {
            return i;
        }
    }
    return -1;
}

int doesTeamExist(char teamName[]) {
    for (int i = 0; i < teamsCounter; i++) {
        if (strcmp(teamName, teams[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

void changePassword(int teamIndex) {
    char newPassword[30];
    
    printf(FCYAN "Enter new password: ");
    scanf("%s", newPassword);
    
    strcpy(teamManagers[teamIndex].password, newPassword);
    printf(FRED "Password changed successfully!\n");
    
    writeToFiles();
}

int doesTeamManagerEmailExist(char email[]) {
    for (int i = 0; i < teamsCounter; i++) {
        if (strcmp(email, teamManagers[i].email) == 0) {
            return i;
        }
    }
    return -1;
}

void addTeam() {
    char teamName[30];
    char managerEmail[30];
    
    printf(FBLUE "Enter team name: ");
    fflush(stdin);
    scanf("%s", teamName);
    
    if (doesTeamExist(teamName) != -1) {
        printf(FRED "Team already exists!\n");
        return;
    }
    
    printf(FORANGE "Enter team manager email: ");
    scanf("%s", managerEmail);
    
    if (doesTeamManagerEmailExist(managerEmail) != -1) {
        printf(FRED "Email is already registered!\n");
        return;
    }
    
    int teamId = createIdForTeam();
    
    struct Team team;
    strcpy(team.managerEmail, managerEmail);
    strcpy(team.name, teamName);
    team.isReadyToPlay = 0;
    team.budget = INITIAL_BUDGET;
    team.teamId = teamId;
    team.teamPlayerCounter = 0;
    
    struct TeamManager teamManager;
    strcpy(teamManager.email, managerEmail);
    strcpy(teamManager.password, managerEmail);
    teamManager.teamId = teamId;
    
    teams[teamsCounter] = team;
    teamManagers[teamsCounter] = teamManager;
    teamsCounter++;
    
    printf(FORANGE "\nSuccessfully added!\nName: %s, Manager Email: %s, Budget: %d$, ID: %d\n\n",
           teamName, managerEmail, team.budget, teamId);
    
    writeToFiles();
}

int createIdForTeam() {
    srand(time(0));
    while (1) {
        int random = (rand() % 9000) + 1000;
        int exists = 0;
        
        for (int i = 0; i < teamsCounter; i++) {
            if (teams[i].teamId == random) {
                exists = 1;
                break;
            }
        }
        
        if (!exists) return random;
    }
}

int createIdForPlayer() {
    srand(time(0));
    while (1) {
        int random = (rand() % 9000) + 1000;
        int exists = 0;
        
        for (int i = 0; i < playersCounter; i++) {
            if (players[i].id == random) {
                exists = 1;
                break;
            }
        }
        
        if (!exists) return random;
    }
}

void addPlayer() {
    char name[30];
    int attackPower, defensePower, value;
    
    printf(FMAGNETA "Enter name: ");
    fflush(stdin);
    gets(name);
    
    printf("Enter attack power (0-100): ");
    scanf("%d", &attackPower);
    while (attackPower < 0 || attackPower > 100) {
        printf(FRED "Attack power must be in range [0-100]: ");
        scanf("%d", &attackPower);
    }
    
    printf(FMAGNETA "Enter defense power (0-100): ");
    scanf("%d", &defensePower);
    while (defensePower < 0 || defensePower > 100) {
        printf(FRED "Defense power must be in range [0-100]: ");
        scanf("%d", &defensePower);
    }
    
    printf(FMAGNETA "Enter value (10-20): ");
    scanf("%d", &value);
    while (value < 10 || value > 20) {
        printf(FRED "Value must be in range [10-20]: ");
        scanf("%d", &value);
    }
    
    int id = createIdForPlayer();
    
    struct Player player;
    strcpy(player.name, name);
    strcpy(player.teamName, "free agent");
    player.attackPower = attackPower;
    player.defensePower = defensePower;
    player.id = id;
    player.value = value;
    
    players[playersCounter] = player;
    playersCounter++;
    
    printf(FRED "Player successfully added!\n");
    showPlayer(player);
    
    writeToFiles();
}

void showTeams() {
    if (teamsCounter == 0) {
        printf(FRED "No teams exist!\n");
        return;
    }
    
    for (int i = 0; i < teamsCounter; i++) {
        showTeam(teams[i]);
    }
}

void showPlayers() {
    if (playersCounter == 0) {
        printf(FRED "No players exist!\n");
        return;
    }
    
    for (int i = 0; i < playersCounter; i++) {
        showPlayer(players[i]);
    }
}

void showTeam(struct Team team) {
    printf(FYELLOW "Name: %s, Manager Email: %s, ID: %d, Budget: %d, Players: %d\n",
           team.name, team.managerEmail, team.teamId, team.budget, team.teamPlayerCounter);
    printf(FORANGE "\n----------------------------------\n");
    printf("Players Info:\n");
    printf("----------------------------------\n");
    
    for (int i = 0; i < team.teamPlayerCounter; i++) {
        showPlayer(team.teamPlayers[i]);
    }
}

void showPlayer(struct Player player) {
    printf(FBLACK "\nID: %-3d  Name: %-10s  Value: %-3d  Attack: %-3d  Defense: %-3d  Team: %-10s\n",
           player.id, player.name, player.value, player.attackPower, 
           player.defensePower, player.teamName);
}

int getPlayerAttack(struct Player player) {
    return player.attackPower;
}

int getPlayerDefense(struct Player player) {
    return player.defensePower;
}

void startLeague() {
    if (doesLeagueStarted == 1) {
        printf(FRED "League has already started!\n");
        return;
    }
    
    if (teamsCounter < MIN_TEAMS) {
        printf(FRED "At least 4 teams are required!\n");
        return;
    }
    
    int readyTeamsCount = 0;
    for (int i = 0; i < teamsCounter; i++) {
        if (teams[i].isReadyToPlay == 1) {
            readyTeamsCount++;
        }
    }
    
    if (readyTeamsCount < MIN_TEAMS) {
        printf(FRED "Teams have not submitted squads yet!\n");
        return;
    }
    
    chooseTeamsForLeague();
}

void chooseTeamsForLeague() {
    for (int i = 0; i < teamsCounter; i++) {
        printf("No. %d:\n", i);
        printf(FCYAN "Name: %s, Manager Email: %s, Budget: %d$, ID: %d\n\n",
               teams[i].name, teams[i].managerEmail, teams[i].budget, teams[i].teamId);
    }
    
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        printf(FBLUE "Enter team %d number to join the league: ", i + 1);
        int index;
        scanf("%d", &index);
        
        if (index < 0 || index >= teamsCounter) {
            printf(FRED "Invalid index, try again!\n");
            i--;
        } else {
            league.teams[i] = teams[index];
        }
    }
    
    createFixtures();
    
    for (int i = 0; i < LEAGUE_WEEKS; i++) {
        league.matchCounter[i] = 0;
    }
    
    printf(FRED "League successfully created!\n");
    doesLeagueStarted = 1;
    
    writeToFiles();
}

void setupFixtures() {
    struct Team dummy;
    dummy.teamId = -1;
    
    // Team 0 fixtures
    league.teamsOpponents[0][0] = league.teams[1];
    league.teamsOpponents[0][1] = league.teams[2];
    league.teamsOpponents[0][2] = league.teams[3];
    league.teamsOpponents[0][3] = league.teams[1];
    league.teamsOpponents[0][4] = league.teams[2];
    league.teamsOpponents[0][5] = league.teams[3];
    
    // Team 1 fixtures
    strcpy(dummy.name, league.teams[0].name);
    league.teamsOpponents[1][0] = dummy;
    league.teamsOpponents[1][1] = league.teams[2];
    league.teamsOpponents[1][2] = league.teams[3];
    league.teamsOpponents[1][3] = dummy;
    league.teamsOpponents[1][4] = league.teams[2];
    league.teamsOpponents[1][5] = league.teams[3];
    
    // Team 2 fixtures
    league.teamsOpponents[2][0] = league.teams[3];
    strcpy(dummy.name, league.teams[0].name);
    league.teamsOpponents[2][1] = dummy;
    strcpy(dummy.name, league.teams[1].name);
    league.teamsOpponents[2][2] = dummy;
    league.teamsOpponents[2][3] = league.teams[3];
    strcpy(dummy.name, league.teams[0].name);
    league.teamsOpponents[2][4] = dummy;
    strcpy(dummy.name, league.teams[1].name);
    league.teamsOpponents[2][5] = dummy;
    
    // Team 3 fixtures
    strcpy(dummy.name, league.teams[2].name);
    league.teamsOpponents[3][0] = dummy;
    strcpy(dummy.name, league.teams[1].name);
    league.teamsOpponents[3][1] = dummy;
    strcpy(dummy.name, league.teams[0].name);
    league.teamsOpponents[3][2] = dummy;
    strcpy(dummy.name, league.teams[2].name);
    league.teamsOpponents[3][3] = dummy;
    strcpy(dummy.name, league.teams[1].name);
    league.teamsOpponents[3][4] = dummy;
    strcpy(dummy.name, league.teams[0].name);
    league.teamsOpponents[3][5] = dummy;
}

void createFixtures() {
    setupFixtures();
    
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        league.teams[i].gf = 0;
        league.teams[i].ga = 0;
        league.teams[i].gd = 0;
        league.teams[i].pts = 0;
        league.teams[i].wins = 0;
        league.teams[i].loses = 0;
        league.teams[i].draws = 0;
    }
    
    for (int i = 0; i < LEAGUE_WEEKS; i++) {
        league.matchCounter[i] = 0;
    }
    
    writeToFiles();
}

void playWeekGames() {
    if (doesLeagueStarted == 0) {
        printf(FRED "League has not started yet!\n");
        return;
    }
    
    int canStartWeek = 1;
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        if (league.teams[i].isReadyToPlay == 0) {
            canStartWeek = 0;
            printf(FGREEN "Team '%s' with manager '%s' has not submitted squad\n",
                   league.teams[i].name, league.teams[i].managerEmail);
        }
    }
    
    if (canStartWeek == 1) {
        startWeekGames();
    } else {
        printf(FRED "Cannot start week %d\n", currentWeek);
    }
    
    writeToFiles();
}

void updatePlayerStats(int teamIndex, int startPlayer, int endPlayer) {
    for (int i = startPlayer; i < endPlayer; i++) {
        int actualIndex = i % SQUAD_SIZE;
        
        for (int j = 0; j < playersCounter; j++) {
            if (strcmp(league.teams[teamIndex].teamPlayers[actualIndex].name, 
                      players[j].name) == 0) {
                league.teams[teamIndex].teamPlayers[actualIndex].attackPower = 
                    getPlayerAttack(players[j]);
                league.teams[teamIndex].teamPlayers[actualIndex].defensePower = 
                    getPlayerDefense(players[j]);
                teams[teamIndex].teamPlayers[actualIndex].attackPower = 
                    getPlayerAttack(players[j]);
                teams[teamIndex].teamPlayers[actualIndex].defensePower = 
                    getPlayerDefense(players[j]);
                break;
            }
        }
    }
}

int shouldSkipMatch(int homeIdx, int awayIdx, int week) {
    int pairIndex = -1;
    
    if ((homeIdx == 0 && awayIdx == 3) || (homeIdx == 3 && awayIdx == 0)) pairIndex = 0;
    else if ((homeIdx == 0 && awayIdx == 2) || (homeIdx == 2 && awayIdx == 0)) pairIndex = 1;
    else if ((homeIdx == 0 && awayIdx == 1) || (homeIdx == 1 && awayIdx == 0)) pairIndex = 2;
    else if ((homeIdx == 1 && awayIdx == 3) || (homeIdx == 3 && awayIdx == 1)) pairIndex = 3;
    else if ((homeIdx == 1 && awayIdx == 2) || (homeIdx == 2 && awayIdx == 1)) pairIndex = 4;
    else if ((homeIdx == 2 && awayIdx == 3) || (homeIdx == 3 && awayIdx == 2)) pairIndex = 5;
    
    if (pairIndex != -1) {
        league.matchCounter[week] += 1;
        if (league.matchCounter[week] >= 3) {
            return 1;
        }
    }
    
    return 0;
}

void showFutureGames(int week) {
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        struct Team homeTeam = league.teams[i];
        struct Team awayTeam = league.teamsOpponents[i][week];
        
        int j;
        for (j = 0; j < LEAGUE_TEAMS; j++) {
            if (strcmp(league.teams[j].name, awayTeam.name) == 0) break;
        }
        
        if (awayTeam.teamId == -1 || i == j) continue;
        
        if (shouldSkipMatch(i, j, week)) continue;
        
        printf(FRED "Match: %s vs %s\n\n", homeTeam.name, awayTeam.name);
    }
}

void applyFatigue(int homeIdx, int awayIdx) {
    int homeGlobalIdx = doesTeamExist(league.teams[homeIdx].name);
    int awayGlobalIdx = doesTeamExist(league.teams[awayIdx].name);
    
    for (int k = 0; k < PLAYING_SQUAD; k++) {
        // Reduce stats for league teams
        league.teams[homeIdx].teamPlayers[k].attackPower -= FATIGUE_REDUCTION;
        if (league.teams[homeIdx].teamPlayers[k].attackPower < 0)
            league.teams[homeIdx].teamPlayers[k].attackPower = 0;
            
        league.teams[homeIdx].teamPlayers[k].defensePower -= FATIGUE_REDUCTION;
        if (league.teams[homeIdx].teamPlayers[k].defensePower < 0)
            league.teams[homeIdx].teamPlayers[k].defensePower = 0;
            
        league.teams[awayIdx].teamPlayers[k].attackPower -= FATIGUE_REDUCTION;
        if (league.teams[awayIdx].teamPlayers[k].attackPower < 0)
            league.teams[awayIdx].teamPlayers[k].attackPower = 0;
            
        league.teams[awayIdx].teamPlayers[k].defensePower -= FATIGUE_REDUCTION;
        if (league.teams[awayIdx].teamPlayers[k].defensePower < 0)
            league.teams[awayIdx].teamPlayers[k].defensePower = 0;
        
        // Reduce stats for global teams
        teams[awayGlobalIdx].teamPlayers[k].attackPower -= FATIGUE_REDUCTION;
        if (teams[awayGlobalIdx].teamPlayers[k].attackPower < 0)
            teams[awayGlobalIdx].teamPlayers[k].attackPower = 0;
            
        teams[homeGlobalIdx].teamPlayers[k].defensePower -= FATIGUE_REDUCTION;
        if (teams[homeGlobalIdx].teamPlayers[k].defensePower < 0)
            teams[homeGlobalIdx].teamPlayers[k].defensePower = 0;
            
        teams[homeGlobalIdx].teamPlayers[k].attackPower -= FATIGUE_REDUCTION;
        if (teams[homeGlobalIdx].teamPlayers[k].attackPower < 0)
            teams[homeGlobalIdx].teamPlayers[k].attackPower = 0;
            
        teams[awayGlobalIdx].teamPlayers[k].defensePower -= FATIGUE_REDUCTION;
        if (teams[awayGlobalIdx].teamPlayers[k].defensePower < 0)
            teams[awayGlobalIdx].teamPlayers[k].defensePower = 0;
    }
}

void startWeekGames() {
    // Update player stats before games
    updatePlayerStats(0, 5, 8);
    updatePlayerStats(1, 13, 16);
    updatePlayerStats(2, 21, 24);
    updatePlayerStats(3, 29, 32);
    
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        struct Team homeTeam = league.teams[i];
        struct Team awayTeam = league.teamsOpponents[i][currentWeek];
        
        int j;
        for (j = 0; j < LEAGUE_TEAMS; j++) {
            if (strcmp(league.teams[j].name, awayTeam.name) == 0) break;
        }
        
        if (awayTeam.teamId == -1 || i == j) continue;
        if (shouldSkipMatch(i, j, currentWeek)) continue;
        
        // Calculate goals
        int goalHome = (getTeamAttack(homeTeam) - getTeamDefense(awayTeam)) / 100;
        int goalAway = (getTeamAttack(awayTeam) - getTeamDefense(homeTeam)) / 100;
        
        if (goalHome < 0) goalHome = 0;
        if (goalAway < 0) goalAway = 0;
        
        // Update statistics
        league.teams[i].gf += goalHome;
        league.teams[i].ga += goalAway;
        league.teams[i].gd += goalHome - goalAway;
        
        league.teams[j].gf += goalAway;
        league.teams[j].ga += goalHome;
        league.teams[j].gd += goalAway - goalHome;
        
        int homeGlobalIdx = doesTeamExist(league.teams[i].name);
        int awayGlobalIdx = doesTeamExist(league.teams[j].name);
        
        // Update points and budget
        if (goalAway > goalHome) {
            league.teams[j].pts += 3;
            teams[awayGlobalIdx].budget += WIN_BUDGET;
            teams[homeGlobalIdx].budget += LOSE_BUDGET;
            league.teams[j].wins++;
            league.teams[i].loses++;
        } else if (goalAway < goalHome) {
            league.teams[i].pts += 3;
            teams[awayGlobalIdx].budget += LOSE_BUDGET;
            teams[homeGlobalIdx].budget += WIN_BUDGET;
            league.teams[i].wins++;
            league.teams[j].loses++;
        } else {
            league.teams[i].pts += 1;
            league.teams[j].pts += 1;
            teams[awayGlobalIdx].budget += DRAW_BUDGET;
            teams[homeGlobalIdx].budget += DRAW_BUDGET;
            league.teams[i].draws++;
            league.teams[j].draws++;
        }
        
        applyFatigue(i, j);
    }
    
    printf(FLIMOIE "Week %d games played!\n", currentWeek + 1);
    currentWeek++;
    
    // Reset player stats at specific weeks
    if (currentWeek == 3 || currentWeek == 6) {
        updatePlayerStats(0, 0, 8);
        updatePlayerStats(1, 8, 16);
        updatePlayerStats(2, 16, 24);
        updatePlayerStats(3, 24, 32);
        doesTransferWindowOpened = 0;
    }
    
    writeToFiles();
}

int getTeamAttack(struct Team team) {
    int sum = 0;
    for (int i = 0; i < team.teamPlayerCounter; i++) {
        sum += team.teamPlayers[i].attackPower;
    }
    return sum;
}

int getTeamDefense(struct Team team) {
    int sum = 0;
    for (int i = 0; i < team.teamPlayerCounter; i++) {
        sum += team.teamPlayers[i].defensePower;
    }
    return sum;
}

void showStandings() {
    printf(FCYAN "%-20s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s\n",
           "Name", "Week", "Wins", "Pts", "Draws", "Loses", "GF", "GA", "GD");
    
    sortTeams(league.teams);
    
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        printf(FBLUE "%-20s%-12d%-12d%-12d%-12d%-12d%-12d%-12d%-12d\n",
               league.teams[i].name, currentWeek, league.teams[i].wins, 
               league.teams[i].pts, league.teams[i].draws, league.teams[i].loses,
               league.teams[i].gf, league.teams[i].ga, league.teams[i].gd);
    }
}

void leagueStanding() {
    printf(FLIMOIE "%-20s%-12s%-12s%-12s%-12s%-12s%-12s%-12s%-12s\n",
           "Name", "Week", "Wins", "Pts", "Draws", "Loses", "GF", "GA", "GD");
    
    struct Team tempTeams[LEAGUE_TEAMS];
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        tempTeams[i] = league.teams[i];
    }
    
    sortTeams(tempTeams);
    
    for (int i = 0; i < LEAGUE_TEAMS; i++) {
        printf(FYELLOW "%-20s%-12d%-12d%-12d%-12d%-12d%-12d%-12d%-12d\n",
               tempTeams[i].name, currentWeek, tempTeams[i].wins,
               tempTeams[i].pts, tempTeams[i].draws, tempTeams[i].loses,
               tempTeams[i].gf, tempTeams[i].ga, tempTeams[i].gd);
    }
}

void openCloseTransferWindows() {
    if (currentWeek != 3 && currentWeek != 6) {
        printf(FRED "\n\nTransfer window not available yet\n\n");
        return;
    }
    
    if (doesTransferWindowOpened == 0) {
        printf(FRED "Transfer window is now open!\n");
        doesTransferWindowOpened = 1;
    } else {
        printf(FRED "Transfer window is now closed!\n");
        doesTransferWindowOpened = 0;
    }
}

void swapLeagueTeams(struct Team *teams, int i, int j) {
    struct Team temp = teams[i];
    teams[i] = teams[j];
    teams[j] = temp;
}

void sortTeams(struct Team *teams) {
    for (int i = 0; i < LEAGUE_TEAMS - 1; i++) {
        for (int j = i + 1; j < LEAGUE_TEAMS; j++) {
            if (teams[i].pts < teams[j].pts) {
                swapLeagueTeams(teams, i, j);
            } else if (teams[i].pts == teams[j].pts) {
                if (teams[i].gd < teams[j].gd) {
                    swapLeagueTeams(teams, i, j);
                } else if (teams[i].gd == teams[j].gd) {
                    if (teams[i].gf < teams[j].gf) {
                        swapLeagueTeams(teams, i, j);
                    } else if (teams[i].gf == teams[j].gf && teams[i].ga > teams[j].ga) {
                        swapLeagueTeams(teams, i, j);
                    }
                }
            }
        }
    }
}

void announceChampion() {
    if (!doesLeagueStarted) {
        printf(FRED "League has not started!\n");
        return;
    }
    
    for (int i = 0; i < LEAGUE_WEEKS; i++) {
        startWeekGames();
    }
    
    showStandings();
    endScreen();
}

void sortPlayers() {
    for (int i = 0; i < playersCounter; i++) {
        for (int j = 0; j < playersCounter - 1; j++) {
            if (players[j].value < players[j + 1].value) {
                struct Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }
}

void buyPlayer(int index) {
    sortPlayers();
    
    if (currentWeek == 1 || currentWeek == 2 || currentWeek == 4 || currentWeek == 5) {
        printf(FRED "Transfer window is closed!\n");
        return;
    }
    
    for (int i = 0; i < playersCounter; i++) {
        if (strcmp(players[i].teamName, "free agent") == 0) {
            printf(FCYAN "No. %d\n", i);
            showPlayer(players[i]);
        }
    }
    
    int playerIndex;
    scanf("%d", &playerIndex);
    
    if (players[playerIndex].value <= teams[index].budget) {
        strcpy(players[playerIndex].teamName, teams[index].name);
        teams[index].teamPlayers[teams[index].teamPlayerCounter] = players[playerIndex];
        teams[index].teamPlayerCounter++;
        teams[index].budget -= players[playerIndex].value;
        printf(FRED "Player successfully purchased!\n");
    } else {
        printf(FRED "Insufficient budget!\n");
    }
}

void sellPlayer(int index) {
    sortPlayers();
    
    if (currentWeek == 1 || currentWeek == 2 || currentWeek == 4 || currentWeek == 5) {
        printf(FRED "Transfer window is closed!\n");
        return;
    }
    
    for (int i = 0; i < playersCounter; i++) {
        if (strcmp(players[i].teamName, teams[index].name) == 0) {
            printf(FCYAN "No. %d\n", i);
            showPlayer(players[i]);
        }
    }
    
    int playerIndex;
    scanf("%d", &playerIndex);
    
    for (int i = 0; i < teams[index].teamPlayerCounter; i++) {
        if (teams[index].teamPlayers[i].id == players[playerIndex].id) {
            break;
        }
    }
    
    teams[index].teamPlayerCounter--;
    teams[index].budget += players[playerIndex].value;
    strcpy(players[playerIndex].teamName, "free agent");
    
    printf(FRED "Player successfully sold!\n");
}

void selectSquad(int index) {
    if (teams[index].teamPlayerCounter < PLAYING_SQUAD) {
        printf(FRED "You need at least 5 players!\n");
        return;
    }
    
    for (int i = 0; i < teams[index].teamPlayerCounter; i++) {
        printf(FYELLOW "No. %d\n", i);
        showPlayer(teams[index].teamPlayers[i]);
    }
    
    int selectedPlayers[PLAYING_SQUAD];
    int position = 0;
    
    for (int i = 0; i < PLAYING_SQUAD; i++) {
        scanf("%d", &selectedPlayers[i]);
        
        if (selectedPlayers[i] < 0 || selectedPlayers[i] >= teams[index].teamPlayerCounter) {
            printf(FRED "Invalid index!\n");
            i--;
        } else {
            struct Player temp = teams[index].teamPlayers[position];
            teams[index].teamPlayers[position] = teams[index].teamPlayers[selectedPlayers[i]];
            teams[index].teamPlayers[selectedPlayers[i]] = temp;
            
            struct Player temp2 = league.teams[index].teamPlayers[position];
            league.teams[index].teamPlayers[position] = 
                league.teams[index].teamPlayers[selectedPlayers[i]];
            league.teams[index].teamPlayers[selectedPlayers[i]] = temp2;
            
            position++;
        }
    }
    
    printf(FRED "Squad successfully submitted!\n");
    teams[index].isReadyToPlay = 1;
}

void showFixtures(int index) {
    if (!doesLeagueStarted) {
        printf(FRED "\n\nLeague has not started\n\n");
        return;
    }
    
    setupFixtures();
    
    for (int week = 0; week < LEAGUE_WEEKS; week++) {
        showFutureGames(week);
    }
    
    for (int i = 0; i < LEAGUE_WEEKS; i++) {
        league.matchCounterPreview[i] = 0;
    }
    
    printf("\n\n\n");
    leagueStanding();
    printf("\n\n\n");
}

void showNextOpponent(int index) {
    int leagueIndex;
    for (leagueIndex = 0; leagueIndex < LEAGUE_TEAMS; leagueIndex++) {
        if (strcmp(league.teams[leagueIndex].name, teams[index].name) == 0) {
            break;
        }
    }
    
    showTeam(league.teamsOpponents[leagueIndex][currentWeek]);
    
    int oppAttack = getTeamAttack(league.teamsOpponents[leagueIndex][currentWeek]);
    int oppDefense = getTeamDefense(league.teamsOpponents[leagueIndex][currentWeek]);
    
    printf("\nOpponent attack power: %d\n", oppAttack);
    printf("Opponent defense power: %d\n\n\n", oppDefense);
}

void updatePassword(int index) {
    char newPassword[30];
    
    printf(FORANGE "Enter new password: ");
    scanf("%s", newPassword);
    
    strcpy(teamManagers[index].password, newPassword);
    printf("Password successfully updated!\n");
}

void readFromFiles() {
    FILE *file;
    
    if ((file = fopen("league.dat", "rb"))) {
        fread(&league, sizeof(struct League), 1, file);
        fclose(file);
    }
    
    if ((file = fopen("teams.dat", "rb"))) {
        while (fread(&teams[teamsCounter], sizeof(struct Team), 1, file)) {
            teamsCounter++;
        }
        fclose(file);
    }
    
    if ((file = fopen("teamManagers.dat", "rb"))) {
        int i = 0;
        while (fread(&teamManagers[i], sizeof(struct TeamManager), 1, file)) {
            i++;
        }
        fclose(file);
    }
    
    if ((file = fopen("players.dat", "rb"))) {
        while (fread(&players[playersCounter], sizeof(struct Player), 1, file)) {
            playersCounter++;
        }
        fclose(file);
    }
    
    if ((file = fopen("leagueOtherInfo.dat", "rb"))) {
        fread(&leagueOtherInfo, sizeof(struct LeagueOtherInfo), 1, file);
        fclose(file);
    }
    
    currentWeek = leagueOtherInfo.currentWeek;
    doesLeagueStarted = leagueOtherInfo.doesLeagueStarted;
    doesTransferWindowOpened = leagueOtherInfo.doesTransferWindowOpened;
}

void writeToFiles() {
    FILE *file;
    
    file = fopen("league.dat", "wb");
    fwrite(&league, sizeof(struct League), 1, file);
    fclose(file);
    
    file = fopen("teams.dat", "wb");
    for (int i = 0; i < teamsCounter; i++) {
        fwrite(&teams[i], sizeof(struct Team), 1, file);
    }
    fclose(file);
    
    file = fopen("teamManagers.dat", "wb");
    for (int i = 0; i < teamsCounter; i++) {
        fwrite(&teamManagers[i], sizeof(struct TeamManager), 1, file);
    }
    fclose(file);
    
    file = fopen("players.dat", "wb");
    for (int i = 0; i < playersCounter; i++) {
        fwrite(&players[i], sizeof(struct Player), 1, file);
    }
    fclose(file);
    
    file = fopen("leagueOtherInfo.dat", "wb");
    fwrite(&leagueOtherInfo, sizeof(struct LeagueOtherInfo), 1, file);
    fclose(file);
}
