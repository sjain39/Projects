//<applet width=600 height=600 code=Othello></applet>
/*
 * @ Othello.java 	
 *
 * Copyright (c) Shubham Jain, Tanuj Sharma.
 * #All Rights Reserved.
 */

import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;

/**
 * This class is for the Othello game.
 */
public class Othello extends java.applet.Applet implements ActionListener
{	
	
	/* Heuristic Values=
	 1.00 	-0.25 0.10 0.05 0.05 0.10 -0.25  1.00
	-0.25 	-0.25 0.01 0.01 0.01 0.01 -0.25 -0.25
	 0.10 	 0.01 0.05 0.02 0.02 0.05  0.01  0.10
	 0.05 	 0.01 0.02 0.01 0.01 0.02  0.01  0.05
	 0.05 	 0.01 0.02 0.01 0.01 0.02  0.01  0.05
	 0.10 	 0.01 0.05 0.02 0.02 0.05  0.01  0.10
	-0.25 	-0.25 0.01 0.01 0.01 0.01 -0.25 -0.25
	 1.00 	-0.25 0.10 0.05 0.05 0.10 -0.25  1.00
	 */
	
	public double heur[] [] = new double [8][8] ;
	
	public double a[][]=new double [][] {{1,-0.25,0.1,0.05},{-0.25,-0.25,0.01,0.01},{0.1,0.01,0.05,0.02},{0.05,0.01,0.02,0.01}};
	
	
	
	public boolean heuristicArray(){
		int q , j ;
		for(   q = 0; q < 4 ; q++ )
		{
			for(  j=0; j<4 ;j++)
			{
				heur[q][j]=a[q][j] * 50;
				heur[7-q][j]=heur[q][j];
				heur[q][7-j]=heur[q][j];
				heur[7-q][7-j]=heur[q][j];
			}
		}
		return true ;
	}
	
	Button NEWGAME;
    Button UNDOMOVE;
    Button HINT;  

	final int BLACK =  1;	
  	final int WHITE = -1;
  	final int EMPTY =  0;
  	final int RED = 2;
  	final int GRAY = 3;
  	final int GREEN = 4;
  	
  	final int WIDTH = 480;
  	
  	private final int UPPER = 0;
  	private final int LOWER = 1;
  	private final int RIGHT = 2;
  	private final int LEFT  = 3;
  	private final int UPPERLEFT = 4;
  	private final int UPPERRIGHT = 5;
  	private final int LOWERRIGHT = 6;
  	private final int LOWERLEFT	= 7;
  	boolean direction[] = {false, false, false, false, false, false, false, false};
  	
  	protected int intelligence[][] = new int[8][8];
  	protected int undoArr[][] = new int[8][8] ;
  	
  	public int turn,flag=0,soch=0;  
  	/**
   	* The othello board. matrix of 8 * 8.<br>
   	*/
  	protected int stone[][],stoneanim[][];
  	/**
  	 * Number of stones on the board<br>
    */
  	protected int counter_black = 0, counter_white = 0;
  	
  	OthelloPlayer computer;
  	
  	public void init()
  		{
    		stone = new int[8][8];
    		stoneanim = new int[8][8];
    		for (int i = 0; i < 8; i++)
    		{
      			for (int j = 0; j < 8; j++)
      			{
					intelligence[i][j] = EMPTY ;
					stone[i][j] = EMPTY;
					stoneanim[i][j] = EMPTY;
					undoArr[i][j] = EMPTY ;
      			}
    		}
    		heuristicArray();
    		stone[3][3] = BLACK; stone[4][3] = WHITE;
    		stone[3][4] = WHITE; stone[4][4] = BLACK;
   	 
    		countStone();
    		turn = BLACK;
    	
    		computer = new OthelloPlayer(this);
  		}
    
    public void paint(Graphics g)
    	{
       		drawBoard(g);
    		for(int i = 0; i < 8; i++)
    		{
    		  for(int j = 0; j < 8; j++)
      			{
	  				stoneanim[i][j] = EMPTY;											
      			}	
    		}
    		if(flag>0)
    		{
    		for(int i = 0; i < 8; i++)
    		{
    		  for(int j = 0; j < 8; j++)
      			{
					if (stone[i][j] != computer.stonecopy[i][j])
						{
						if(stone[i][j] == WHITE)
	  							stoneanim[i][j] = GRAY;
	  					else
	  						if(stone[i][j] == BLACK)
	  							stoneanim[i][j]=RED;
	  					if(stoneanim[i][j] != 0 && computer.stonecopy[i][j] == 0)
	  						stoneanim[i][j] = 0;				
						}
      			}	
    		}
    		for(int i = 0; i < 8; i++)
    		{
    		  for(int j = 0; j < 8; j++)
      			{
					if (stone[i][j] != stoneanim[i][j] && stoneanim[i][j] == 0)
						{
	  					stoneanim[i][j] = stone[i][j];				
						}							
      			}	
    		}
    		for(int i = 0; i < 8; i++)
    		{
    		  for(int j = 0; j < 8; j++)
      			{
      				if (stone[i][j] != 0)
					fadeStone(i,j,g);					
      			}	
    		}
    		drawCountStone(g);
    		try 
    			{
      				Thread.sleep(1000);
    			}
    		catch(Exception e)
    			{
    			}
    		}
    		for(int i = 0; i < 8; i++)
    		{
    		  for(int j = 0; j < 8; j++)
      			if (stone[i][j] != 0)//computer.stonecopy[i][j])
						{
	  					drawStone(i, j, g);
	  					flag++;
						}	
    		}			
    		    		
    		drawCountStone(g);

    		if (counter_black + counter_white == 64)
    			{
      				endGame();
      			}
    		else
    			if (!checkAll(turn))
    			{
      				endGame();
      			}
      			else
    				drawTurn(g);		
  		}
  		/**
   		* Draw the Othello board<br>
   		*/
  	public void drawBoard (Graphics g)
  		{    	
  			setSize(600,600);
    		setBackground(Color.orange);
    		Panel buttonpanel=new Panel();
    		NEWGAME = new Button("New Game"); 
  			UNDOMOVE = new Button("Undo Move");
  			HINT = new Button("Hint Move");
  			NEWGAME.addActionListener(this);
  			UNDOMOVE.addActionListener(this);
  			HINT.addActionListener(this);
 			buttonpanel.setLayout(new FlowLayout(FlowLayout.CENTER));
  			buttonpanel.add(NEWGAME);
  			buttonpanel.add(UNDOMOVE);
  			buttonpanel.add(HINT);
  			setLayout(new BorderLayout());
  			add(buttonpanel,"South");
  			setVisible(true);
    		g.setColor(Color.black);
    		g.drawLine(0,0, 0,WIDTH);		
    		g.drawLine(WIDTH,0, WIDTH,WIDTH);	
    		g.drawLine(0,0, WIDTH,0);		
    		g.drawLine(0,WIDTH, WIDTH,WIDTH);	
   
    		for(int i = 1; i < 8; i++)
    		{
      			g.drawLine(WIDTH*i/8,0, WIDTH*i/8,WIDTH);
      			g.drawLine(0,WIDTH*i/8, WIDTH,WIDTH*i/8);
    		}
  			
  		}

  		
  	public void fadeStone(int column, int row, Graphics g)
  		{
    		if (stoneanim[column][row] == BLACK)
    			{
      				g.setColor(Color.black);
    			}
    		else
    			if (stoneanim[column][row] == WHITE)
    				{
      					g.setColor(Color.white);
    				}
    			else
    				if(stoneanim[column][row] == RED)
    				{
    					g.setColor(Color.red);
    				}
    				else
    					if(stoneanim[column][row] == GRAY)
    						{
    							g.setColor(Color.gray);
    						}
    					else
    						if(stoneanim[column][row] == GREEN)
    						{
    							g.setColor(Color.gray);
    						}
    		g.fillOval(column * WIDTH / 8 + 10, row * WIDTH / 8 + 10, WIDTH / 12, WIDTH / 12);
  		}
  			
  		/**
   		* Paint stones on the board.<br>
   		*/
  	public void drawStone(int column, int row, Graphics g)
  		{
    		if (stone[column][row] == BLACK)
    			{
      				g.setColor(Color.black);
      				g.fillOval(column * WIDTH / 8 + 10, row * WIDTH / 8 + 10, WIDTH / 12, WIDTH / 12);
    			}
    		else
    			if (stone[column][row] == WHITE)
    				{
      					g.setColor(Color.white);
      					g.fillOval(column * WIDTH / 8 + 10, row * WIDTH / 8 + 10, WIDTH / 12, WIDTH / 12);
    				}
    			else
    				if (stone[column][row] == GREEN)
    				{
    					g.setColor(Color.yellow);
    					g.fillRect(column * WIDTH / 8 + 10, row * WIDTH / 8 + 10, WIDTH / 12, WIDTH / 12);
    					stone[column][row] = 0;
    				}
    		//g.fillOval(column * WIDTH / 8 + 10, row * WIDTH / 8 + 10, WIDTH / 12, WIDTH / 12);
  		}
		/**
   		* Display which trun.
   		*/
  	void drawTurn(Graphics g)
  		{
    		String black = "Black";		
    		String white = "White";		
    		String comment = " Player turn";
    		Font big = new Font("ARIAL", Font.BOLD+Font.ITALIC, 14);		
    
    		g.setFont(big);
    		g.setColor(Color.black);
    		if (turn == BLACK)
    			{
    				g.setColor(Color.black);
      				g.drawString(black + comment, WIDTH/2, WIDTH + 35);
      				showStatus("Your turn!");
      				g.fillOval(WIDTH/2 - 30, WIDTH+20, 20, 20);
    			}
    		else
    			{
      				g.drawString(white + comment, WIDTH/2, WIDTH + 35);
      				showStatus("Compurter's turn!");
      				g.setColor(Color.white);
      				g.fillOval(WIDTH/2 - 30, WIDTH+20, 20, 20);
    			}		
  		}
  		/**
   		* Show win or lose.<br>
   		*/
  	public void showWinner(Graphics g)
  		{
    	//	g.clearRect(0, 0, WIDTH+1, WIDTH+1);
    		String black = "YOU WON";		
    		String white = "YOU LOST";
    		String draw = "GAME DRAW";		
    		Font big = new Font("sanserif", Font.BOLD, 70);		
    
    		g.setFont(big);
    		g.setColor(Color.red);

    		if (counter_black > counter_white)
    			{
      			//	for(int  i = 0; i < 255; i++)
      			//	{
					//	g.fillRect(0, 0, WIDTH+1, WIDTH+1);
						g.drawString(black, 90, 550);
      					showStatus("You Won!");
      			//	}
    			}
    		else
    			if (counter_black < counter_white)
    				{
      				
							g.drawString(white, 90, 550);
      						showStatus("You Lost!");
      			/*			try 
    						{
      							Thread.sleep(500);
    						}
    						catch(Exception e)
    						{
    				//		}*/      					
				    }
				else
					{
      					g.drawString(draw, 90, 550);
      					showStatus("Game Draw!");
    				}
  		}
  		/**
   		*  this function Count the number of stones on the board.
   		*/
  	void countStone()
  		{
    		counter_black = 0;
    		counter_white = 0;
    
    		for(int i = 0; i < 8; i++)
    			{
      				for(int j = 0; j < 8; j++)
      					{
							if(stone[i][j] == BLACK) counter_black++;
							if(stone[i][j] == WHITE) counter_white++;
      					}
    			}
			/*if(!checkAll(turn) || !checkAll(-turn))
			{
				endGame();
			}*/
    		if (counter_black + counter_white == 64)
    			{
      				endGame();   		
    			}
  		}
	
	
		/**
   		* Display winner. This method is called when the game is finished.
   		*/
   		
   		
  	public void endGame()
  		{
  			Graphics g=getGraphics();    	
    		if (counter_black > counter_white)
    			{
    				showWinner(g);
    			}
    		else
    			if (counter_black < counter_white)
    				{
    					showWinner(g);
    				}
    			else
    				{
    					showWinner(g);
    				}
    		//	repaint();
    		/*	try 
    			{
      			Thread.sleep(500);
    			}
    			catch(Exception e)
    			{
    			}*/
  		}
  	
  	
  		/**
   		* Display the number of stones.<br>
   		*/
  	
  	
  	void drawCountStone(Graphics g)
  		{
    		g.setColor(Color.white);
    		g.fill3DRect(WIDTH+15, 130, 30,20, false);
    		g.fill3DRect(WIDTH+15, 190, 30,20, false);
    		g.fill3DRect(WIDTH+5, 160, 20,20, true);
    		g.setColor(Color.black);
    		g.fill3DRect(WIDTH+5, 100, 20,20, true);
    		g.drawString("Black", WIDTH+30, 115);		
    		g.drawString("White", WIDTH+30, 175);
   			g.drawString(Integer.toString(counter_black), WIDTH+20, 145); 
    		g.drawString(Integer.toString(counter_white), WIDTH+20, 205);
    		String game = "Unbeatable Othello";
    		String version = "Version 1.0";		
    		//Font big = new Font("sanserif", Font.BOLD+Font.CENTER_BASELINE, 14);		
    		//g.setFont(big);
    		//g.setColor(Color.red);
    		g.drawString(game, WIDTH+5, WIDTH-20);
    		g.drawString(version, WIDTH+30, WIDTH);
    		g.drawString("Credits : Shubham Jain", WIDTH-15, WIDTH+25);
    		g.drawString("Tanuj Sharma", WIDTH+35, WIDTH+45);
    		g.drawString("Deepak Joshi", WIDTH+35, WIDTH+65);
  		}
  		/**
   		* When mouse button is released.
   		*/
  	public boolean mouseUp(Event e, int x, int y)
  		{
    		int column = (int)(x / (WIDTH / 8));
    		int row	   = (int)(y / (WIDTH / 8));
    
    		if (turn == BLACK)
    			{
    				/*if(!checkAll(turn))
      					{
      						endGame();      						
      					}
      				else*/
      					undoCopy();
      					computer.boardcopy();
      					if (checkStone(column, row, turn) == true)
      					{
							turnStone(column, row, turn);
							turn = - turn;
							countStone();
							repaint();
							update(getGraphics());
	
							try 
								{
	  							Thread.sleep(500);
								}
							catch (Exception excep)
								{
								}
      					}	
    			}
    					

    		if (turn == WHITE)
    			{
    				try 
    				{
      					Thread.sleep(500);
    				}
    				catch(Exception evt)
    				{
    				}
    				if (!checkAll(turn))
    				{
    					System.out.println("GAME KHATAM KARO");
      					endGame();
    				}
    				else
    				{
      					computer.MKLdecide();
      					countStone();
      					repaint();
      					update(getGraphics());
    				}		
      				

    			}
    		return true;
  		}
  	int copyBlack  =0 , copyWhite = 0;	
  	
  	public void undoCopy(){
  		
  		for( int i =0 ; i<8 ; i++)
  		{
  			for(int j =0 ; j<8; j++ )
  			{
  				undoArr[i] [j] = stone[i] [j] ;
  			}
  		}
  		
  		copyBlack = counter_black ; 
  		copyWhite = counter_white ;	
  	
  	}	
  		
  		public void undoMove(){
  		if( counter_black + counter_white > 4){
  		
  		for( int i =0 ; i<8 ; i++)
  		{
  			for(int j =0 ; j<8; j++ )
  			{
  			stone[i] [j] =	undoArr[i] [j]  ;
  			}
  		}
  		
  		counter_black = copyBlack ;
  		counter_white = copyWhite ;
  		}
  	}	
  				
  	public void run()
  		{
    		try
    			{
      			Thread.sleep(500); 
    			}
    		catch (Exception e)
    			{
    			}
  		}
    
    public boolean checkAll(int turn)
    	{
    		for(int i = 0; i < 8; i++)
    			{
      				for(int j = 0; j < 8; j++)
      					{
							if(checkStone(i, j, turn))
								{
	  							return true;
								}
      					}
    			}
    		return false;
  		}
  /* this function check whether a stone can be placed or not */
  	public boolean checkStone(int column, int row, int color)
  		{
    	int i, j;
    	for (i = 0; i < 8; i++)
    		{
      			direction[i] = false;
    		}
    	if(stone[column][row] != 0)
    		{
        		return false;
    		}
    	else
    		{ 
    			if (column > 1 && stone[column-1][row] == -color)
    				{
           				for (i = column-2; i > 0 && stone[i][row] == -color; i--);
						if (stone[i][row] == color)
							{
	  							direction[LEFT] = true;
							}
      				}
      			if (column < 6 && stone[column+1][row] == -color)
      				{
						for (i = column+2; i < 7 && stone[i][row] == -color; i++);
						if (stone[i][row] == color)
							{
	  							direction[RIGHT] = true;
							}
      				}
      			if (row > 1 && stone[column][row-1] == -color)
      				{
						for (j = row-2; j > 0 && stone[column][j] == -color; j--);
						if (stone[column][j] == color)
							{
	  							direction[UPPER] = true;
							}
      				}
      			if (row < 6 && stone[column][row+1] == -color)
      				{
						for (j = row+2; j < 7 && stone[column][j] == -color; j++);
						if (stone[column][j] == color)
							{
	  						direction[LOWER] = true;
							}
      				}
      			if (column > 1 && row > 1 && stone[column-1][row-1] == -color)
      				{
						for (i = column-2, j = row-2; i > 0 && j > 0 && stone[i][j] == -color; i--, j--);
						if (stone[i][j] == color)
							{
	  						direction[UPPERLEFT] = true;
							}
      				}
      			if (column < 6 && row > 1 && stone[column+1][row-1] == -color)
      				{
						for (i = column+2, j = row-2; i < 7 && j > 0 && stone[i][j] == -color; i++, j--);
						if (stone[i][j] == color)
							{	  
	  							direction[UPPERRIGHT] = true;
							}
      				}
      			if (column < 6 && row < 6 && stone[column+1][row+1] == -color)
      				{
						for (i = column+2, j = row+2; i < 7 && j < 7 && stone[i][j] == -color; i++, j++);
						if (stone[i][j] == color)
							{
	  							direction[LOWERRIGHT] = true;
							}
      				}
      			if (column > 1 && row < 6 && stone[column-1][row+1] == -color)
      				{
						for (i = column-2, j = row+2; i > 0 && j < 7 && stone[i][j] == -color; i--, j++);
						if (stone[i][j] == color)
							{
								direction[LOWERLEFT] = true;
							}
      				} 
      					
      			for (i = 0; i < 8; i++)
      				{
						if (direction[i] == true)
							{
	  							return true;
							}
      				}

      			return false;
    		}
  		}
  
  	public int turnStone(int column, int row, int color)
  		{
    	stone[column][row] = color;
    	int i,j;
    	
    	int flipcount = 0 ;
    	
    	if (direction[LEFT] == true)
    		{
      			for (i = column-1; stone[i][row] != color; i--)
      				{
						stone[i][row] = - stone[i][row];
						flipcount++;
						//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[RIGHT] == true)
    		{
      			for (i = column + 1; stone[i][row] != color; i++)
      				{
						stone[i][row] = - stone[i][row];
						flipcount++;
						//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[UPPER] == true)
    		{
      			for (j = row - 1; stone[column][j] != color; j--)
      				{
						stone[column][j] = - stone[column][j];
						flipcount++;
						//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[LOWER] == true)
    		{
      			for (j = row + 1; stone[column][j] != color; j++)
      				{
						stone[column][j] = - stone[column][j];
      					flipcount++;
      					//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[UPPERLEFT] == true)
    		{
      			for (i = column-1, j = row-1; stone[i][j] != color; i--, j--)
      				{
						stone[i][j] = - stone[i][j];
      					flipcount++;
      					//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[UPPERRIGHT] == true)
    		{
      			for (i = column+1, j = row-1; stone[i][j] != color; i++, j--)
      				{
						stone[i][j] = - stone[i][j];
      					flipcount++;
      					//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[LOWERRIGHT] == true)
    		{
      			for (i = column+1, j = row+1; stone[i][j] != color; i++, j++)
      				{
						stone[i][j] = - stone[i][j];
      					flipcount++;
      					//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	if (direction[LOWERLEFT] == true)
    		{
      			for (i = column-1, j = row+1; stone[i][j] != color; i--, j++)
      				{
						stone[i][j] = - stone[i][j];
      					flipcount++;
      					//System.out.println("flipcount = "+flipcount);
      				}
    		}
    	return flipcount ;	
    	}
    public void actionPerformed(ActionEvent evt)
	{
		String str=evt.getActionCommand();
		if (str.equals("New Game"))
			{
			//System.out.println("NAYA KHEL");	
			init();
			repaint();
      		//update(getGraphics());
			}
		else
			if(str.equals("Undo Move"))
			{
			//System.out.println("PICHLI CHAAL");
			undoMove();
			repaint();
      		//pdate(getGraphics());
			}
			else
				if(str.equals("Hint Move"))
				{
				//System.out.println("MADAD KARO");
				//rang=1;
				computer.MKLdecide();
				repaint();
      			//update(getGraphics());
				}
				else
					if(str.equals("Start Game"))
					{
						//soch=1;
						init();
						//soch=1;
						repaint();
					}
	}
}



 class OthelloPlayer {
  Othello parent;

  OthelloPlayer(Othello parent){
    this.parent = parent;
  }
 
 	
 protected int stonecopy[][] = new int[8][8] ;

 protected int turncopy ;

 public void BKLdecide()
  	{
  		int i=0,j=0;
    if (parent.checkAll(parent.turn) == true)
    	{
      	do
      		{
			i = (int)(Math.random() * 8);
			j = (int)(Math.random() * 8);
      		} 
      		while (! parent.checkStone(i, j, parent.turn));
      
      		parent.turnStone(i, j, parent.turn);
    	}
    parent.turn = - parent.turn;
  }

 public void boardcopy()
 {
 	turncopy = parent.turn ;

 	for ( int i=0 ; i<8 ; i++)
 	{
 		for(int j=0 ; j<8 ; j++)
 			stonecopy[i][j] = parent.stone[i][j] ;
 	}

 }

 public void undomove ()
 {
 	parent.turn = turncopy ;
 	for ( int i=0 ; i<8 ; i++)
 	{
 		for(int j=0 ; j<8 ; j++)
 			parent.stone[i][j] = stonecopy[i][j] ;
 	}
 }
 
 public void hint(int x, int y)
 {
 	parent.stone[x][y] = parent.GREEN;
 }
 
  public void MKLdecide(){
  	
  	int temp = 0 ;
  	boardcopy();
  	int bestmoveI = 0 , bestmoveJ = 0 ;
  	if (parent.checkAll(parent.turn) == true) {
      for (int i=0 ; i<8 ; i++)
      	{
      	for(int j=0 ; j<8 ; j++)
      			{
      		 	if(parent.checkStone(i ,j , parent.turn) == true)
      				{
						temp = 1 ;	
      					parent.intelligence[i][j] = parent.turnStone(i ,j ,parent.turn);
						undomove();
      				}
      			}
     	}  	
  		
  double max = 0;
  int check = 0 ;
  for (int m = 0 ; m < 8 ; m++ )
  {
  	for (int n = 0 ; n < 8 ; n++ )
  	{
  		if( ((parent.heur[m][n])*( parent.intelligence[m][n ])) > max )
  		{
  			max =  (double)(parent.heur[m][n])*( parent.intelligence[m][n ]);
  			bestmoveI = m ;
  			bestmoveJ = n ;
  			check = 1;
  		}
  		
  		
  	}
  }
 if( check == 0  && temp == 1)
 {
 
  for (int m = 0 ; m < 8 ; m++ )
  {
  	for (int n = 0 ; n < 8 ; n++ )
  	{
  		if( (( parent.intelligence[m][n ])) > max )
  		{
  			max =  ( parent.intelligence[m][n ]);
  			bestmoveI = m ;
  			bestmoveJ = n ;
  			
  		}
  		
  		
  	}
  }
  
 }
  
  	if(parent.turn == parent.WHITE){
  			if(parent.checkStone(bestmoveI , bestmoveJ , parent.turn)){
					parent.turnStone( bestmoveI , bestmoveJ , turncopy);
					parent.turn = - parent.turn ;
			
			}
  	}
  	// blinking of hint u have bestmovei = i ,bestmovej = j 
  	else{
  		hint(bestmoveI, bestmoveJ);
  		}
  	
  	for (int i = 0; i < 8; i++)
    		{
      			for (int j = 0; j < 8; j++)
      			{
					parent.intelligence[i][j] = 0 ;
					
				}
    		}
  	
  }
 else
 	parent.endGame();
 }  
 }