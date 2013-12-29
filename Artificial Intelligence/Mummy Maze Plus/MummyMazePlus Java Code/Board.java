//package Maze;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class Board extends JPanel implements ActionListener {

	private Timer timer;
	private Map m;
	private Player p;
	private Villain v;
	private Heart h1, h2, h3, h;
	private int mapHeight = 20;
	private int mapWidth = 20;
	private int floor = 1;
	private int level = 1;
	private int health = 3;
	private long time = 2*3600;	//set countdown timer from 2 minutes
	private long score = 100;

	//fields for if user wins
	private boolean kill = false;
	private boolean win = false;
	private boolean lost = false;
	private boolean displayWelcomeScreen = true;
	private boolean displayMainScreen = false;
	private boolean displaySelectLevel = false;
	private boolean displayHighScore = false;
	private boolean displayMap = false;
	private boolean displayOption = false;
	private boolean pause = false;
	//private boolean pMoved = true;

	public Board(){
		m = new Map();
		p = new Player();
		v = new Villain();
		h1 = new Heart();
		h2 = new Heart();
		h3 = new Heart();
		addKeyListener(new actionListener());
		setFocusable(true);

		timer = new Timer(25, this);
		timer.start();
	}//end of Board

	//action for winning
	public void actionPerformed(ActionEvent e){
		time--;
		if((time/60) == 0){
			lost = true;
		}
		//Mummy catches player
		mummyCatchesPlayer();
		//Map response when player moves in pitfall, ladder, firepole, finishing flag
		playerMovesSpecialCell();		
		repaint();
	}//end of actionPerformed
	
	public void mummyCatchesPlayer(){
		if(p.getTileX() == v.getVillainX() && p.getTileY() == v.getVillainY() && !lost && !kill)
		{
			System.out.println("GAME OVER MUMMY GOT YOU");
			kill = true;
			//decrease health
			decreaseHealth();
		}
	}
	
	public void decreaseHealth(){
		health--;
		if(health == 2)
			h3.makeWall();
		else if(health == 1)
			h2.makeWall();
		else if(health == 0)
		{
			kill = false;
			lost = true;
			level=1;
			floor=1;
			score-=20;
		}
	}
	
	public void playerMovesSpecialCell(){
		if(m.getMap(p.getTileX(), p.getTileY()).equals("o")){
			decreaseHealth();
			p.resetPlayer();
			v.resetVillian();
			System.out.println("Health Decreases");
		}
		
		if(m.getMap(p.getTileX(), p.getTileY()).equals("f")){
			win = true;
			System.out.println("WON LEVEL: "+level);
			level++;
			score+=100;
			floor=1;
			m.openFile("Map"+level+"-1");
			p.resetPlayer();
			v.resetVillian();
		}
		
		if(m.getMap(p.getTileX(), p.getTileY()).equals("p")){
			//DO STUFF TO MOVE FLOOR DOWN
			System.out.println("PLAYER IS MOVING FLOOR DOWN");
			floor--;
			m.openFile("Map1-"+floor);
			v.resetVillian();
		}
		
		//l is ladder
		if(m.getMap(p.getTileX(), p.getTileY()).equals("l")){
			//DO STUFF TO MOVE FLOOR UP
			System.out.println("PLAYER IS MOVING FLOOR UP");
			floor++;
			m.openFile("Map1-"+floor);
			v.resetVillian();
		}
	}

	public void paint(Graphics g){
		super.paint(g);

		if(!win){
			if(displayWelcomeScreen){
				g.drawImage(m.getWelcomeScreen(), 0, 0, 640, 640, null);
			}
			else{
				if(displayMainScreen){
					g.drawImage(m.getMainScreen(), 0, 0, 640, 640, null);
				}
				else{
					if(displaySelectLevel){
						g.drawImage(m.getSelectLevelScreen(), 0, 0, 640, 640, null);
					}
					else if(displayHighScore){
						g.drawImage(m.getScoreScreen(), 0, 0, 640, 640, null);
						g.setFont(new Font("Arial", Font.BOLD, 18));
					    g.setColor(Color.white);
					    int verticalSpace = 150;
						for(int i = 0; i < mapHeight; i++){
							g.drawString(m.getScores(i), 5*32, verticalSpace+i*32);
						}
					}
					else{
						if(displayMap){
							for(int i = 0; i< mapHeight; i++){
								for(int j = 0; j< mapWidth; j++){
									if(m.getMap(i,j).equals("f"))
										g.drawImage(m.getFinish(), i * 32, j * 32, 32, 32, null);
									if(m.getMap(i,j).equals("g") )
										g.drawImage(m.getGrass(), i * 32, j * 32, 32, 32, null);
									if(m.getMap(i,j).equals("w") )
										g.drawImage(m.getWall(), i * 32, j * 32, 32, 32, null);
									if(m.getMap(i,j).equals("e") )
										g.drawImage(m.getWater(), i * 32, j * 32, 32, 32, null);
									if(m.getMap(i,j).equals("l"))
										g.drawImage(m.getLadder(), i*32, j*32, 32, 32, null);
									if(m.getMap(i,j).equals("p"))
										g.drawImage(m.getFirepole(), i*32, j*32, 32, 32, null);
									if(m.getMap(i,j).equals("o"))
										g.drawImage(m.getPitfall(), i*32, j*32, 32, 32, null);
									//draws player
									g.drawImage(p.getPlayer(),p.getTileX() * 32, p.getTileY() * 32, 32, 32, null);
									//draws villain
									g.drawImage(v.getVillain(),v.getVillainX() * 32, v.getVillainY() * 32, 32, 32, null);
									//draws heart
									g.drawImage(h1.getHeart(), 0 * 32, 19 * 32, 32, 32, null);
									g.drawImage(h2.getHeart(), 1 * 32, 19 * 32, 32, 32, null);
									g.drawImage(h3.getHeart(), 2 * 32, 19 * 32, 32, 32, null);
									
									g.setFont(new Font("Arial", Font.BOLD, 28));
									g.setColor(Color.white);
									g.drawString("Floor: "+floor, 16*32, 1*32);
									g.drawString("Level: "+level, 0*32, 1*32);
									g.drawString("Score: "+score, 8*32, 1*32);
									
									g.setFont(new Font("Arial", Font.BOLD, 28));
									g.setColor(Color.yellow);
									long minutes = (time/3600);
									long seconds = (time/60)%60;
									g.drawString("Time Left: "+Long.toString(minutes)+"m"+Long.toString(seconds)+"s", 12*32, 20*32);
									if(kill)
									{
										g.drawImage(m.getBlood(), p.getTileX()*32, p.getTileY()*32, 32, 32, null);
									    g.setFont(new Font("Arial", Font.BOLD, 24));
									    g.setColor(Color.black);
										g.drawString("Mummy killed you! Start again", 5*32, 10*32);
									}
								}
							}
						}
						else
							System.out.println("DISPLAY MAP IS SET TO FALSE");
					}
				}
			}
		}//end of if !win
		
		if(win){
			g.drawImage(p.getPlayerFinished(), 0, 0, 640, 640, null);
			g.setFont(new Font("Arial", Font.BOLD, 22));
			g.setColor(Color.black);
			g.drawString("Press 'N' to move next level", 180, 50);
		}//end of if win

		if(lost && !kill)
		{
			g.drawImage(m.getGameover(), 0, 0, 640, 640, null);
			g.setFont(new Font("Arial", Font.BOLD, 22));
			g.setColor(Color.black);
			g.drawString("Press 'Escape' for options", 180, 40);
		}
		
		if(displayOption)
		{
			g.drawImage(m.getOption(), 0, 0, 640, 640, null);
		}
		
		if(pause)
			g.drawImage(m.getPausegame(), 0, 0, 640, 640, null);

	}//end of paint method
	
	public void VM(Villain v){

		int ranNum = (int)(Math.random() *4) +1;
		System.out.println(ranNum);

		if(ranNum == 1){
			if(!m.getMap(v.getVillainX(), v.getVillainY() - 1).equals("w") )
				v.villainMove(0, -1);
		}
		if(ranNum == 2){
			if(!m.getMap(v.getVillainX(), v.getVillainY() + 1).equals("w") )
				v.villainMove(0, 1);
		}
		if(ranNum == 3){
			if(!m.getMap(v.getVillainX() - 1, v.getVillainY() ).equals("w") )
				v.villainMove(-1, 0);
		}
		if(ranNum == 4){
			if(!m.getMap(v.getVillainX() + 1, v.getVillainY() ).equals("w") )
				v.villainMove(1, 0);
		}
	}


	public class actionListener extends KeyAdapter{
		public void keyPressed(KeyEvent e){
			
			if(kill)
			{
				p.resetPlayer();
				v.resetVillian();
				kill = false;
			}
			
			if(displayWelcomeScreen)
			{
				displayWelcomeScreen = false;
				displayMainScreen = true;
				return;
			}
			
			int keyCode = e.getKeyCode();
			
			if(displayMainScreen)
			{
				if(keyCode == KeyEvent.VK_1){
					displaySelectLevel = true;
				}
				if(keyCode == KeyEvent.VK_2){
					displayHighScore = true;
				}
				if(keyCode == KeyEvent.VK_3){
					System.exit(0);
				}
				displayMainScreen = false;
				return;
			}
			
			if(displaySelectLevel){
				if(keyCode == KeyEvent.VK_1){
					level=1;
					m.openFile("Map1-1");
				}
				if(keyCode == KeyEvent.VK_2){
					level=2;
					m.openFile("Map2-1");
				}
				if(keyCode == KeyEvent.VK_3){
					level=3;
					m.openFile("Map3-1");
				}
				if(keyCode == KeyEvent.VK_4){
					level=4;
					m.openFile("Map4-1");
				}
				if(keyCode == KeyEvent.VK_5){
					level=5;
					m.openFile("Map5-1");
				}
				displaySelectLevel = false;
				displayMap = true;
				return;
			}
			
			if(displayHighScore)
			{
				displayHighScore = false;
				displayOption = true;
				return;
			}

			if(keyCode == KeyEvent.VK_N && !lost){
				h1 = new Heart();
				h2 = new Heart();
				h3 = new Heart();
				time = 7200;
				health = 3;
				kill = false;
				lost = false;
				win = false;
				return;
			}
			
			if(keyCode == KeyEvent.VK_P){
				pause = true;
				return;
			}
			
			if(keyCode == KeyEvent.VK_R){
				pause = false;
				return;
			}
			
			if(keyCode == KeyEvent.VK_ESCAPE){
				//Write code for exit
				displayOption = true;
				return;
			}
			
			if(keyCode == KeyEvent.VK_M){
				h1 = new Heart();
				h2 = new Heart();
				h3 = new Heart();
				time = 7200;
				health = 3;
				kill = false;
				lost = false;
				p.resetPlayer();
				v.resetVillian();
				displayOption = false;
				displayMap = false;
				displayMainScreen = true;
				return;
			}
			
			
			if(keyCode == KeyEvent.VK_Q){
				System.exit(0);
			}
			
			//Handling player and mummy movement
			movementPlayerVillian(keyCode);			
		}//end of keyCode

	}//end of class actionListener
	
	public void movementPlayerVillian(int keyCode)
	{
		//Handling moving player with controls UP,DOWN,LEFT,RIGHT
		if(keyCode == KeyEvent.VK_UP){
			//if you touch a "w" or wall, the move does not occur
			if(!m.getMap(p.getTileX(), p.getTileY() - 1).equals("w") && !m.getMap(p.getTileX(), p.getTileY()-1).equals("e")){
				p.move(0, -1);
				VM_AI(v);
			}
		}
		
		//if you press down, move down
		if(keyCode == KeyEvent.VK_DOWN){
			if(!m.getMap(p.getTileX(), p.getTileY()+1).equals("w") && !m.getMap(p.getTileX(), p.getTileY()+1).equals("e")){
				p.move(0, 1);
				VM_AI(v);
			}
		}
		//if you press left
		if(keyCode == KeyEvent.VK_LEFT){
			if(!m.getMap(p.getTileX()-1, p.getTileY() ).equals("w") && !m.getMap(p.getTileX()-1, p.getTileY()).equals("e")){
				p.move(-1, 0);
				VM_AI(v);
			}
		}
		//if you press right
		if(keyCode == KeyEvent.VK_RIGHT){
			if(!m.getMap(p.getTileX()+1, p.getTileY() ).equals("w") && !m.getMap(p.getTileX()+1, p.getTileY() ).equals("e")){
				p.move(1, 0);
				VM_AI(v);
			}
		}
	}

	public void moveUp(int deltaX, int deltaY){
		if(!m.getMap(v.getVillainX(), v.getVillainY() - 1).equals("w") )
		{
			System.out.println("Mummy Move UP");
			v.villainMove(0, -1);
		}
		else
		{
			System.out.println("Mummy Hits Wall on UP");
			VM(v);
		}
	}

	public void moveDown(int deltaX, int deltaY){
		if(!m.getMap(v.getVillainX(), v.getVillainY() + 1).equals("w") )
		{
			System.out.println("Mummy Move DOWN");
			v.villainMove(0, 1);
		}
		else
		{
			System.out.println("Mummy Hits Wall on DOWN");
			VM(v);
		}
	}

	public void moveRight(int deltaX, int deltaY){
		if(!m.getMap(v.getVillainX() + 1, v.getVillainY() ).equals("w") )
		{
			System.out.println("Mummy Move RIGHT");
			v.villainMove(1,  0);
		}
		else
		{
			System.out.println("Mummy Hits Wall on RIGHT");
			VM(v);
		}
	}

	public void moveLeft(int deltaX, int deltaY){
		if(!m.getMap(v.getVillainX() - 1, v.getVillainY() ).equals("w") )
		{
			System.out.println("Mummy Move LEFT");
			v.villainMove(-1, 0);
		}
		else
		{
			System.out.println("Mummy Hits Wall on LEFT");
			VM(v);
		}
	}

	public void VM_AI(Villain v){

		int deltaX = p.getTileX() - v.getVillainX();
		int deltaY = p.getTileY() - v.getVillainY();

		deltaX = -deltaX;
		deltaY = -deltaY;
		System.out.println("DeltaX = "+deltaX);
		System.out.println("DeltaY = "+deltaY);

		if(Math.abs(deltaX) < Math.abs(deltaY))
		{
			if(deltaX < 0)
				moveRight(deltaX, deltaY);
			else
			{
				if(deltaX > 0)
					moveLeft(deltaX, deltaY);
				else
				{
					if(deltaY > 0)
						moveUp(deltaX, deltaY);
					else
						moveDown(deltaX, deltaY);
				}
			}
		}
		else if(Math.abs(deltaX) > Math.abs(deltaY))
		{
			if(deltaY < 0)
				moveDown(deltaX, deltaY);
			else
			{
				if(deltaY > 0)
					moveUp(deltaX, deltaY);
				else
				{
					if(deltaX > 0)
						moveLeft(deltaX, deltaY);
					else
						moveRight(deltaX, deltaY);
				}
			}
		}
		else
		{
			System.out.println("FTW");
			//repaint();
		}
		System.out.println("Player Position: ("+p.getTileX()+","+p.getTileY()+")");
		System.out.println("Mummy Position: ("+v.getVillainX()+","+v.getVillainY()+")");
	}
}//end of class Board