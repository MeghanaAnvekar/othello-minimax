#include<iostream>
#include<stdio.h>
#include<ctype.h>
#include<vector>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

using namespace std;

struct state
{
	char grid[8][8];
	bool valid;
	double alpha, beta;
	int x,y,c;
	vector<state*> successors;	
};

bool canmove(char self, char opp, char *str);
bool isLegalMove(char self, char opp, char grid[8][8], int startx, int starty);
int num_valid_moves(char self, char opp, char grid[8][8]);
double dynamic_heuristic_evaluation_function(char grid[8][8]);

void init_game(char grid[8][8]);
void next_move(char grid[8][8]);
void print_board(char grid[8][8]);

int is_game_over(char grid[8][8],char self,char opp);
void normalize(char grid[8][8]);
void outflank(char grid[8][8],char self,char opp,int x,int y);

void min_plays(char grid[8][8]);
void max_plays(char grid[8][8],int l);

void init_state(state* s);
void copy_state(state* d,state* s);
void alpha_beta_search(state* root,int l);
void max_value(state* root,int level);
bool min_value(state* root,int level,int p,int q);

state isMoveAllowed(char grid[8][8], int startx, int starty);
void print_score();
void calc_score(char grid[8][8]);
void destroy(state* s);
int max_pieces = 2,min_pieces = 2;
int count = 0;
void print_state(state * s)
{
	cout<<"\n##############################################\n";
	print_board(s->grid);
	cout<<"X : "<<s->x<<" Y : "<<s->y<<" beta : "<<s->beta<<" alpha: "<<s->alpha<<"\n";
	cout<<"\n##############################################\n";
}
int main()
{
  
  char grid[8][8];
  int l;
  init_game(grid);
  cout<<"\nEnter level\n\t1 -> easy\n\t2 -> medium\n\t3-> hard\n\nLEVEl....";
  cin>>	l;
  print_score();
  print_board(grid);
  int result = 0;
  
 while((result = is_game_over(grid,'b','w')) == -2)
  {
 	
 	min_plays(grid);
  	
  	if(is_game_over(grid,'w','b') == -2)
  	{
  		
  		max_plays(grid,l);
  	}
  	
  }
  
  cout<<"\n************************************************************************************************\n";
  
  cout<<"\n      RESULT : ";
  

  switch(result)
  {
  	case 0: cout<< "draw ";break;
  	case 2: cout<<" you won :)";break;
  	case 1: cout<<" better luck next time :)";break;
  	
  }
  
  cout<<"\n************************************************************************************************\n";
  return 0;

}
void destroy(state * root)
{
	while(root->successors.size())
	{
		destroy(root->successors[0]);
		root->successors.erase(root->successors.begin());
	}
	
	delete root;
}
void copy_state(state* d,state* s)
{
	for(int i  = 0;i < 8;++i)
 			for( int j = 0;j < 8;++j)
 				d->grid[i][j] = s->grid[i][j];
 	d->valid = s->valid;
 	d->alpha = s->alpha;
 	d->beta = d->beta;
 	d->successors = s->successors;
	
}

void calc_score(char grid[8][8])
{
	int max = 0,min = 0;
	
	for(int i = 0; i < 8;++i)
		for(int j = 0; j < 8;++j)
		{
			if(tolower(grid[i][j]) == 'w')
				max++;
			else if(tolower(grid[i][j]) == 'b')
				min++;
		}
		
	max_pieces = max;
	min_pieces = min;
}
void print_score()
{
	cout<<"\n::SCORE::  b : "<<min_pieces<< "     w : "<<max_pieces<<"\n";
	cout<<"---------------------------------------------------\n\n";
}

void alpha_beta_search(state* root,int l)
{
	max_value(root,l);
	
	for(auto s:root->successors)
	{
		if(root->alpha == s->beta)
		{
			root->x = s->x;
			root->y = s->y;
			break;
		} 
	}
}



void max_value(state* root,int level)
{
	if(level > 0)
	{
		for(int x = 0; x < 8;++x)
			for(int y = 0;y < 8;++y)
			{						
				
				if(isLegalMove('w','b', root->grid, x, y))
				{
					state* s = new state();
					init_state(s);
					for (int i = 0;i < 8;++i)
						for(int j = 0; j < 8;++j)
							s->grid[i][j] = root->grid[i][j];
					s->grid[x][y] = 'w';
					s->x = x;
					s->y = y;
					s->valid = true;
					outflank(s->grid,'w','b',x,y);
					
					normalize(s->grid);
					
					root->successors.push_back(s); //min level
					bool is_min_move_possible = false;
					for(int p = 0; p < 8;++p)
						for(int q = 0; q < 8;++q)
						{
							
							if(min_value(s,level-1,p,q)) //leaf level max 
							{
								is_min_move_possible = true;
								if(s->beta > s->successors[s->successors.size()-1]->alpha)
								{	
									s->beta = s->successors[s->successors.size()-1]->alpha;
									if(root->alpha < s->beta)
									{
										root->alpha = s->beta;
										root->x = s->x;
										root->y = s->y;
									}
									else
									{
										//prune branches by exiting loop
										 p =8,q = 8;
									}
								}
							}
						}
					
					if(!is_min_move_possible)
					{
						max_value(s,level-2);
					}
						
				}
				
			}
	}
	
	else
	{	
			root->alpha = dynamic_heuristic_evaluation_function(root->grid);			
	}	
}

bool min_value(state* root,int level,int p,int q)
{
	
		if(isLegalMove('b','w', root->grid, p, q))
		{	
			state* s = new state();
			init_state(s);
			for (int i = 0;i < 8;++i)
				for(int j = 0; j < 8;++j)
					s->grid[i][j] = root->grid[i][j];
			
			s->grid[p][q] = 'b';
			s->x = p;
			s->y = q; 
			s->valid = true;
			outflank(s->grid,'b','w',p,q);
			normalize(s->grid);
			max_value(s,level-1);
			root->successors.push_back(s);
			return true;
		}

	return false;	
}

 
 state isMoveAllowed(char grid[8][8], int startx, int starty)   
 {
 	char self = 'w', opp = 'b'; 
 	char str[10]; 
 	int x, y, dx, dy, ctr;
 	state *s = new state();
 	
 	init_state(s);
 	s->valid = false;
 	
 	if (grid[startx][starty] != '-') return *s;
 	
 	for (dy = -1; dy <= 1; dy++)
 		for (dx = -1; dx <= 1; dx++)    
 		{
 			// keep going if both velocities are zero
 			if (!dy && !dx) continue;
 			str[0] = '\0';
 			for (ctr = 1; ctr < 8; ctr++)   
 			{
 				x = startx + ctr*dx;
 				y = starty + ctr*dy;
 				if (x >= 0 && y >= 0 && x<8 && y<8) str[ctr-1] = grid[x][y];
 				else str[ctr-1] = 0;
 			}
 			if (canmove(self, opp, str)) 
 			{				
 				for(int i  = 0;i < 8;++i)
		 			for( int j = 0;j < 8;++j)
		 				s->grid[i][j] = grid[i][j];
				 		s->x = startx;
				 		s->y = starty; 					
				 		s->valid  = true;
 				return *s;	
 			}
 		}
 		
 	return *s;
 }

void init_state(state* s)
{	s->valid = false;
	s->alpha = -99999999;
	s->beta = 99999999;
	s->c = count++;
}


int is_game_over(char grid[8][8],char self,char opp)
{
	if((num_valid_moves(self,opp,grid) == 0 && num_valid_moves(opp,self,grid) == 0) || max_pieces+min_pieces == 64)
	{
		if(max_pieces > min_pieces) return 1;
		else if(max_pieces < min_pieces ) return -1;
		else return 0;
	}
	else
	return -2;
	
}
void print_board(char grid[8][8])
{
  
  cout<<" ";
  for(int j = 0; j < 8;++j)
  {
    cout<<"  "<<j<<"   ";
  }
  cout<<"\n  ";
  for(int j = 0; j < 8;++j)
  {
    cout<<"______";
  }
  cout<<"\n";
  for(int i = 0; i < 8;++i)
  {
    for(int j = 0; j < 8;++j)
    {
      if(j < 1)cout<<" ";
      cout<<"|     ";
    }
    cout<<"|\n";
    for(int j = 0; j < 8;++j)
    {
      if(j < 1)cout<<i;
      cout<<"|  "<<grid[i][j]<<"  ";
    }
    cout<<"|\n";
    for(int j = 0; j < 8;++j)
    {
      if(j < 1)cout<<" ";
      cout<<"|_____";
    }
    cout<<"|\n";

  }
}

void init_game(char grid[8][8])
{
  for(int i = 0; i < 8;++i)
    for(int j = 0; j < 8;++j)
      grid[i][j] = '-';

  grid[3][3]='w';
  grid[3][4]='b';
  grid[4][3]='b';
  grid[4][4]='w';
  
}

void max_plays(char grid[8][8],int l)
{
	state* s = new state();
	init_state(s);
	for(int i  = 0;i < 8;++i)
 		for( int j = 0;j < 8;++j)
			s->grid[i][j] = grid[i][j];
	
	
	alpha_beta_search(s,2*l);

	grid[s->x][s->y] = 'w';
	outflank(grid,'w','b',s->x,s->y);
	cout<<"\nComp plays  X:"<<s->x<<" Y: "<<s->y<<"\n";
	calc_score(grid);
 	print_score();
 	print_board(grid);
 	normalize(grid);
	destroy(s);
}
void min_plays(char grid[8][8])
{
 
  bool valid = false;
   int x,y;
  while(!valid)
  {
	  cout<<"\nYour Turn: ";
	  cout<<"\nX :";
	  cin>>x;
	  cout<<"\nY :";
	  cin>>y;
 	if(isLegalMove('b','w',grid,x,y))
 		valid = true;
 	else
 	{
 		cout<<"\nInvalid move !!!\n";
 	}
 }
 
 grid[x][y] = 'b';
 
 outflank(grid,'b','w',x,y);
 cout<<"After Your move....";
 calc_score(grid);
 print_score();
 print_board(grid);
 normalize(grid);
 
}

void normalize(char grid[8][8])
{
	for(int i = 0;i < 8; ++i)
		for(int j = 0; j < 8; ++j)
			grid[i][j] = tolower(grid[i][j]);
}

void outflank(char grid[8][8],char self,char opp,int x,int y)
{
	int dx,dy;
	
	for(dx = -1; dx <= 1;++dx)
		for(dy  = -1;dy <= 1;++dy)
		{
			if(!dx && !dy)
				continue;
			else
			{
				int pos_x,pos_y;bool found_self = false;
				for(int ctr = 1; ctr < 8 && !found_self;++ctr)
				{
					pos_x = x+ ctr * dx;
					pos_y = y+ ctr * dy;
					
					if(grid[pos_x][pos_y] == self && pos_x >= 0 && pos_x <8 &&  pos_y >= 0 && pos_y <8)
					{
						found_self = true;
						int i,j;
						
						for( i =  pos_x-dx,j = pos_y-dy ;!( i == x && j == y) && i < 8&& j < 8&& i >= 0&& j >=0&&grid[i][j] == opp;i -= dx, j -= dy);
						if(i==x && j == y)
            {
              for( i =  pos_x-dx,j = pos_y-dy ;!( i == x && j == y) && i < 8&& j < 8&& i >= 0&& j >=0&&grid[i][j] == opp;i -= dx, j -= dy)
              {	
                grid[i][j] = toupper(self);	

              }
           }
					}
				}
			}
		}
		
}

bool canmove(char self, char opp, char *str)  {
 	if (str[0] != opp) return false;
 	for (int ctr = 1; ctr < 8; ctr++) {
 		if (str[ctr] == '-') return false;
 		if (tolower(str[ctr]) == self) return true;
 	}
 	return false;
 }

 bool isLegalMove(char self, char opp, char grid[8][8], int startx, int starty)   {
 	if (grid[startx][starty] != '-') return false;
 	char str[10];
 	int x, y, dx, dy, ctr;
 	for (dy = -1; dy <= 1; dy++)
 		for (dx = -1; dx <= 1; dx++)    {
 	// keep going if both velocities are zero
 			if (!dy && !dx) continue;
 			str[0] = '\0';
 			for (ctr = 1; ctr < 8; ctr++)   {
 				x = startx + ctr*dx;
 				y = starty + ctr*dy;
 				if (x >= 0 && y >= 0 && x<8 && y<8) str[ctr-1] = grid[x][y];
 				else str[ctr-1] = 0;
 			}
 			if (canmove(self, opp, str)) return true;
 		}
 	return false;
 }

 int num_valid_moves(char self, char opp, char grid[8][8])   {
 	int count = 0, i, j;
 	for(i=0; i<8; i++)
 		for(j=0; j<8; j++)
 			if(isLegalMove(self, opp, grid, i, j)) count++;
 	return count;
 }


 /*
  * Assuming my_color stores your color and opp_color stores opponent's color
  * '-' indicates an empty square on the board
  * 'b' indicates a black tile and 'w' indicates a white tile on the board
  */
 double dynamic_heuristic_evaluation_function(char grid[8][8])  {
 	int my_tiles = 0, opp_tiles = 0, i, j, k, my_front_tiles = 0, opp_front_tiles = 0, x, y;
 	double p = 0, c = 0, l = 0, m = 0, f = 0, d = 0;
	char my_color = 'w', opp_color='b';
 	int X1[] = {-1, -1, 0, 1, 1, 1, 0, -1};
 	int Y1[] = {0, 1, 1, 1, 0, -1, -1, -1};
 	int V[][8]={
			{20, -3, 11, 8, 8, 11, -3, 20},
  		   	{-3, -7, -4, 1, 1, -4, -7, -3},
     			{11, -4, 2, 2, 2, 2, -4, 11},
		     	{8, 1, 2, -3, -3, 2, 1, 8},
		     	{8, 1, 2, -3, -3, 2, 1, 8},
		     	{11, -4, 2, 2, 2, 2, -4, 11},
		     	{-3, -7, -4, 1, 1, -4, -7, -3},
		     	{20, -3, 11, 8, 8, 11, -3, 20}
		};
 // Piece difference, frontier disks and disk squares
 	for(i=0; i<8; i++)
 		for(j=0; j<8; j++)  {
 			if(grid[i][j] == my_color)  {
 				d += V[i][j];
 				my_tiles++;
 			} else if(grid[i][j] == opp_color)  {
 				d -= V[i][j];
 				opp_tiles++;
 			}
 			if(grid[i][j] != '-')   {
 				for(k=0; k<8; k++)  {
 					x = i + X1[k]; y = j + Y1[k];
 					if(x >= 0 && x < 8 && y >= 0 && y < 8 && grid[x][y] == '-') {
 						if(grid[i][j] == my_color)  my_front_tiles++;
 						else opp_front_tiles++;
 						break;
 					}
 				}
 			}
 		}
 	if(my_tiles > opp_tiles)
 		p = (100.0 * my_tiles)/(my_tiles + opp_tiles);
 	else if(my_tiles < opp_tiles)
 		p = -(100.0 * opp_tiles)/(my_tiles + opp_tiles);
 	else p = 0;

 	if(my_front_tiles > opp_front_tiles)
 		f = -(100.0 * my_front_tiles)/(my_front_tiles + opp_front_tiles);
 	else if(my_front_tiles < opp_front_tiles)
 		f = (100.0 * opp_front_tiles)/(my_front_tiles + opp_front_tiles);
 	else f = 0;

 // Corner occupancy
 	my_tiles = opp_tiles = 0;
 	if(grid[0][0] == my_color) my_tiles++;
 	else if(grid[0][0] == opp_color) opp_tiles++;
 	if(grid[0][7] == my_color) my_tiles++;
 	else if(grid[0][7] == opp_color) opp_tiles++;
 	if(grid[7][0] == my_color) my_tiles++;
 	else if(grid[7][0] == opp_color) opp_tiles++;
 	if(grid[7][7] == my_color) my_tiles++;
 	else if(grid[7][7] == opp_color) opp_tiles++;
 	c = 25 * (my_tiles - opp_tiles);

 // Corner closeness
 	my_tiles = opp_tiles = 0;
 	if(grid[0][0] == '-')   {
 		if(grid[0][1] == my_color) my_tiles++;
 		else if(grid[0][1] == opp_color) opp_tiles++;
 		if(grid[1][1] == my_color) my_tiles++;
 		else if(grid[1][1] == opp_color) opp_tiles++;
 		if(grid[1][0] == my_color) my_tiles++;
 		else if(grid[1][0] == opp_color) opp_tiles++;
 	}
 	if(grid[0][7] == '-')   {
 		if(grid[0][6] == my_color) my_tiles++;
 		else if(grid[0][6] == opp_color) opp_tiles++;
 		if(grid[1][6] == my_color) my_tiles++;
 		else if(grid[1][6] == opp_color) opp_tiles++;
 		if(grid[1][7] == my_color) my_tiles++;
 		else if(grid[1][7] == opp_color) opp_tiles++;
 	}
 	if(grid[7][0] == '-')   {
 		if(grid[7][1] == my_color) my_tiles++;
 		else if(grid[7][1] == opp_color) opp_tiles++;
 		if(grid[6][1] == my_color) my_tiles++;
 		else if(grid[6][1] == opp_color) opp_tiles++;
 		if(grid[6][0] == my_color) my_tiles++;
 		else if(grid[6][0] == opp_color) opp_tiles++;
 	}
 	if(grid[7][7] == '-')   {
 		if(grid[6][7] == my_color) my_tiles++;
 		else if(grid[6][7] == opp_color) opp_tiles++;
 		if(grid[6][6] == my_color) my_tiles++;
 		else if(grid[6][6] == opp_color) opp_tiles++;
 		if(grid[7][6] == my_color) my_tiles++;
 		else if(grid[7][6] == opp_color) opp_tiles++;
 	}
 	l = -12.5 * (my_tiles - opp_tiles);

 // Mobility
 	my_tiles = num_valid_moves(my_color, opp_color, grid);
 	opp_tiles = num_valid_moves(opp_color, my_color, grid);
 	if(my_tiles > opp_tiles)
 		m = (100.0 * my_tiles)/(my_tiles + opp_tiles);
 	else if(my_tiles < opp_tiles)
 		m = -(100.0 * opp_tiles)/(my_tiles + opp_tiles);
 	else m = 0;

 // final weighted score
 	double score = (10 * p) + (801.724 * c) + (382.026 * l) + (78.922 * m) + (74.396 * f) + (10 * d);
 	return score;
 }
