#include <stdio.h>
#include <stdlib.h>

#define N_CARDSET			1
#define N_CARD				52
#define N_DOLLAR			50

#define N_MAX_CARDNUM		13
#define N_MAX_USER			5
#define N_MAX_CARDHOLD		10
#define N_MAX_GO			17
#define N_MAX_BET			5

#define N_MIN_ENDCARD		30
int getCardNum(int cardnum);
void printCard(int cardnum);
void swap (int *x, int *y);
int mixCardTray(void);
int pullCard(void);
int configUser(void);
int betDollar(void);
void offerCards(void);
void printCardInitialStatus(void);
int getAction(int i, int j);
void printUserCardStatus(int user, int cardcnt);
int calcStepResult(int user, int cardcnt);
int checkResult(int roundindex);
int checkWinner(int user);


//card tray object
int CardTray[N_CARDSET*N_CARD]; //[1*52]
int cardIndex = 0;							

//player info
int dollar[N_MAX_USER];						//dollars that each player has
int n_user;									//number of users
int playercardhold[N_MAX_USER] = {2,2,2,2,2};				//card number of each player

//play yard information
int cardhold[N_MAX_USER+1][N_MAX_CARDHOLD];	//cards that currently the players hold, [6][10]
int cardSum[N_MAX_USER+1];					//sum of the cards [5]
int bet[N_MAX_USER];						//current betting  [5]
int gameEnd = 0; 							//game end flag

//some utility functions

//get an integer input from standard input (keyboard)
//return : input integer value
//         (-1 is returned if keyboard input was not integer)
int getIntegerInput(void) {
    int input, num;
    
    num = scanf("%d", &input);
    fflush(stdin);
    if (num != 1) //if it fails to get integer
        input = -1;
    
    return input;
}

int main(int argc, char *argv[]) {
	int roundIndex = 1; 
//	int max_user; = n_user
	int i; // j is card number that each player has.
	
	int k, w; //initialize player card hold, if one player is out of money then game ends
	
	srand((unsigned)time(NULL));
	
	//set the number of players
	configUser();


	//Game initialization --------
	for(i=0; i<n_user; i++) //1. players' dollar
	{
		dollar[i] = N_DOLLAR; // making dollar array 0 to n_user
	}
	
	//2. card tray
	mixCardTray();


	for(i=0;i<52;i++)
	{
		printf("%d  ", CardTray[i]);
	}

	//Game start --------
	do {
		
		printf("\n------------------ Round %d (card index %d)--------------------------\n", roundIndex, cardIndex);
		
		betDollar();
		offerCards(); //1. give cards to all the players
		
		printCardInitialStatus();
		printf("\n------------------ GAME start --------------------------\n");
		
		//each player's turn
		for (i=0; i<=n_user; i++) //each player
		{
			if(i==0)
				printf("\n>>> my turn! -------------\n");
			else if(i>0 && i<n_user)
				printf("\n>>> player %d turn! -------------\n", i);
			else if(i==n_user)
				printf("\n>>> dealer turn! -------------\n");	
			
			while (1) //do until the player dies or player says stop
			{
				printUserCardStatus(i, playercardhold[i]);//print current card status printUserCardStatus();
				cardSum[i] = calcStepResult(i,playercardhold[i]);//check the card status ::: calcStepResult()
				
				if(cardSum[i] < 21)
				{
					printf("sum : %d ", cardSum[i]);
					if(getAction(i,playercardhold[i]) != 0) //GO? STOP? ::: getAction()
					{
						printf("sum : %d ", cardSum[i]);
						break;
					}
				}

				else if(cardSum[i] > 21)
				{
					dollar[i] = dollar[i]-bet[i];
					printf("::: DEAD (sum:%d) ---> -$%d ($%d)\n", cardSum[i], bet[i],dollar[i]);
					break;
				}
				
				else if(cardSum[i] == 21)
				{
					dollar[i] = dollar[i]+ 2*bet[i];
					printf("Black Jack!!!congratulation, you win!! --> +$%d ($%d)\n", bet[i],dollar[i]);
					break;
				}		
				playercardhold[i]++; 					
				//check if the turn ends or not
				//break;
			}

			//show server result
			if(i==n_user)
			{
				printf("[[[[[[[ server result is ....  ");
				if(cardSum[i] <= 21) // just show the record
				{
					printf("%d ]]]]]]]\n", cardSum[i]);
				}
				else// overflow
				{
					printf("overflow!! ]]]]]]]");
				}
			}	
		}
		
		//result
		checkResult(roundIndex);
		
		roundIndex++;
		for (i=0;i<=n_user;i++) //cardSum initialize
		{
			cardSum[i] = 0;
		}
		for(k = 0; k<n_user; k++) // initialize player card hold
			playercardhold[k] = 2;
			
		//escape this for() to end game
		if(cardIndex > N_CARD * N_CARDSET) // if out of card then game ends	
		{
			printf("Out of Card!! Game Ends!\n");
			gameEnd = 1;
		}

		for(w = 0; w<n_user; w++) // if one player is out of money then game ends
		{
			if(dollar[0] <= 0)
			{
				printf("You go bankrupt!! Game Ends!\n");
				gameEnd = 1;
			}
			
			if(w > 0 && dollar[w] <= 0)
			{
				printf("player[%d] goes bankrupt!! Game Ends!\n");
				gameEnd = 1;
			}	
		}	
			
	} while (gameEnd == 0);
	
	
	checkWinner(n_user);
	
	printf("You played %d rounds\n", roundIndex);
	
	return 0;
}

//card processing functions ---------------

//calculate the actual card number in the blackjack game
int getCardNum(int cardnum) {
	
	if(cardnum % (13*N_CARDSET) == 0)
		return 11; // after that, if cardsum is over 21 then -10, if over21 - 10 is over 21 then lose
	else if(cardnum % (13*N_CARDSET) == 1)
		return 2;
	else if(cardnum % (13*N_CARDSET) == 2)
		return 3;
	else if(cardnum % (13*N_CARDSET) == 3)
		return 4;			
	else if(cardnum % (13*N_CARDSET) == 4)
		return 5;
	else if(cardnum % (13*N_CARDSET) == 5)
		return 6;
	else if(cardnum % (13*N_CARDSET) == 6)
		return 7;
	else if(cardnum % (13*N_CARDSET) == 7)
		return 8;
	else if(cardnum % (13*N_CARDSET) == 8)
		return 9;			
	else if(cardnum % (13*N_CARDSET) == 9)
		return 10;
	else if(cardnum % (13*N_CARDSET) == 10)
		return 10;
	else if(cardnum % (13*N_CARDSET) == 11)
		return 10;
	else if(cardnum % (13*N_CARDSET) == 12)
		return 10;
}

//print the card information (e.g. DiaA)
void printCard(int cardnum) {
	
	if(cardnum / (13*N_CARDSET) == 0)
		printf("DIA");
	else if(cardnum / (13*N_CARDSET) == 1)
		printf("SPD");
	else if(cardnum / (13*N_CARDSET) == 2)
		printf("CLV");
	else if(cardnum / (13*N_CARDSET) == 3)
		printf("HRT");			
	
	if(cardnum % (13*N_CARDSET) == 0)
		printf("A ");
	else if(cardnum % (13*N_CARDSET) == 1)
		printf("2 ");
	else if(cardnum % (13*N_CARDSET) == 2)
		printf("3 ");
	else if(cardnum % (13*N_CARDSET) == 3)
		printf("4 ");			
	else if(cardnum % (13*N_CARDSET) == 4)
		printf("5 ");
	else if(cardnum % (13*N_CARDSET) == 5)
		printf("6 ");
	else if(cardnum % (13*N_CARDSET) == 6)
		printf("7 ");
	else if(cardnum % (13*N_CARDSET) == 7)
		printf("8 ");
	else if(cardnum % (13*N_CARDSET) == 8)
		printf("9 ");			
	else if(cardnum % (13*N_CARDSET) == 9)
		printf("10 ");
	else if(cardnum % (13*N_CARDSET) == 10)
		printf("Jack ");
	else if(cardnum % (13*N_CARDSET) == 11)
		printf("Queen ");
	else if(cardnum % (13*N_CARDSET) == 12)
		printf("King ");	
	
}

//card array controllers -------------------------------

//mix swap
void swap (int *x, int *y) 
{ 
	int temp; 
	temp = *x; 
	*x = *y; 
	*y = temp; 
}

//mix the card sets and put in the array, didn't mix only array
int mixCardTray(void) {
	int i;
	int j = 0;
	
	for(i=0; i<N_CARD; i++) 
	{		
		CardTray[i] = j;
		j++;
	}
	
	for(i=0; i<N_CARD; i++) 
	{	
		swap(&CardTray[rand()%52], &CardTray[i]);
	}

	printf("card is mixed and put into the tray\n");
	return 0;
}

//get one card from the tray
int pullCard(void) {
	
	cardIndex++;
	return CardTray[cardIndex-1];

	
}

//playing game functions -----------------------------

//player settiing
int configUser(void) {
	int user; // user = n_user
	do
	{
		printf("How many players do you want?: ");
		scanf("%d", &user);
		if(user > 5)
			printf("Too many players!\n");
		else if(user <= 0)
			printf("Too little players!\n");
		else
			n_user = user;
	}while(user > 5 || user <= 0 );

	return 0;
}


//betting
int betDollar(void) {
	int i, j;
	
    printf("---------BETTING STEP-----------\n");	
	
	do
	{
		printf("your betting(total %d) :", dollar[0]); 
		scanf("%d", &bet[0]); // what
		
		if(bet[0] > dollar[0])
			printf("You don't have that money\n");
		else if(bet[0] <= 0)
			printf("You have to pay more than that\n");
	
	}while(bet[0] > dollar[0] || bet[0] <= 0);
	
	
	for(j=1; j<n_user; j++)
	{
		bet[j] = rand()%N_MAX_BET + 1;  
		printf("player[%d] bets $%d (out of &%d)\n", j, bet[j], dollar[j]);
	}

	printf("----------------------------\n");
	
	return 0;	
}


//offering initial 2 cards
void offerCards(void) {
	int i;
	//1. give two card for each players
	for (i=0;i<n_user;i++)
	{
		cardhold[i][0] = pullCard(); //return CardTray[]
		cardhold[i][1] = pullCard();
	}
	//2. give two card for the operator (dealer)
	cardhold[n_user][0] = pullCard();
	cardhold[n_user][1] = pullCard();

}

//print initial card status
void printCardInitialStatus(void) {
	
	int i;
	
	printf(" ----------- CARD OFFERING ---------------\n");
	printf("server\t: " );
	printf("X ");
	printCard(cardhold[n_user][1]);
	printf("\n");
	printf("-> you\t: " );
	printCard(cardhold[0][0]);
	printCard(cardhold[0][1]);
	printf("\n");
	
	for(i=1; i<n_user; i++)
	{
		printf("-> player[%d]\t:", i);
		printCard(cardhold[i][0]); // player1 has cardhold[][]
		printCard(cardhold[i][1]);
		printf("\n");
	}

}

int getAction(int i, int j) {
	int go=0;
	
	if(i==0) //me
	{
		printf("::: Action? (0 - go, others - stay) :");
		scanf("%d", &go);
			
		if(go == 0)
		{
			cardhold[i][j+1] = pullCard();
			return 0;
		}
		else 
			return 1;
	}
	else //other players
	{
		if (cardSum[i]	< 17 )	
		{	
			cardhold[i][j+1] = pullCard();
			printf(":::GO:::\n");
			return 0;	
		}
		else
		{
			printf(":::STAY:::\n");
			return 1;
		}
	}
	

}


void printUserCardStatus(int user, int cardcnt) {  //cardhold[][]
	int j;
	
	printf("   -> card : ");
	for (j=0;j<cardcnt;j++)
		printCard(cardhold[user][j]);
	printf("\t ::: ");
	
}




// calculate the card sum and see if : 1. under 21, 2. over 21, 3. blackjack
int calcStepResult(int user, int cardcnt) {
	
	int j,anum = 0;
	cardSum[user] = 0;
	
	for(j=0;j<cardcnt;j++)
	{
		if(getCardNum(cardhold[user][j]) == 11)
		{
			anum++;
		}
		else
		{
			cardSum[user] += getCardNum(cardhold[user][j]);
		}
	}


	if(cardSum[user] > 10)
	{
		if(anum != 0)
		{
			cardSum[user] = cardSum[user] + anum * 1;  
		}
	}
	else if(cardSum[user] == 10)
	{
		if (anum > 1)
		{
			cardSum[user] = cardSum[user] + anum * 1;
		}
		else if (anum == 1)
		{
			cardSum[user] = cardSum[user] + 11;
		}
	}
	else // cardsum < 10
	{
		if(anum >= 2)
		{
			cardSum[user] = cardSum[user] + 11 + (anum-1)* 1; 
		}
		else if(anum == 1)
		{
			cardSum[user] = cardSum[user] + 11;
		}
	}
	
	
	return cardSum[user];
	//if CardTray[i] = 13(CLVA) then calculate 1 or 11
}

int checkResult(int roundindex) {
	int i;
	printf("\n-------------------ROUND %d result-----------------\n", roundindex);
	
	for(i=0;i<n_user;i++)
	{
		if(i==0)
			printf("your result : ");
		else if(i>0 && i<n_user)
			printf("\nplayer[%d] result : ", i);
	
		if(cardSum[n_user] > 21) // server lose, anyone who is less 21 wins
		{
			if(cardSum[i] <= 21)
				printf("win (sum: %d) --> $%d",cardSum[i],dollar[i]);
			else 				
				printf("lose due to overflow! ($%d)", dollar[i]);
			
		}
		else if (cardSum[n_user] == 21) //server win - blackjack
		{
			if(cardSum[i] == 21)
				printf("win (sum: %d) --> $%d",cardSum[i],dollar[i]);
			else 				
				printf("lose! ($%d)", dollar[i]);
		}
		else 
		{
			if(cardSum[i] < 21 && cardSum[i] >= cardSum[n_user])//player wins
			{
				printf("win (sum: %d) --> $%d\n",cardSum[i],dollar[i]);
			}
			else if(cardSum[i] < 21 && cardSum[i] < cardSum[n_user]) //server wins
			{
				printf("lose! ($%d)\n", dollar[i]);
			}
			else if(cardSum[i] == 21)
			{
				printf("win (sum: %d) --> $%d\n",cardSum[i],dollar[i]);
			}
			else if(cardSum[i] > 21)
			{
				printf("lose due to overflow! ($%d)\n", dollar[i]);
			}
		}
	}
}

int checkWinner(user) {
	
	int i, userIndex, max_dollar=0;
	
	for(i=0; i<user; i++)
	{
		if(dollar[i] > max_dollar)
			max_dollar = dollar[i];
	}
	
	for(userIndex=0; userIndex<user; userIndex++)
	{
		if(max_dollar = dollar[userIndex])
			break;
	}
	
	if(userIndex == 0)
		printf("Congratulation! You have %d dollars so you win!", dollar[0]);
	else	
		printf("player[%d] has %d dollars so you lose", dollar[userIndex]);
}
