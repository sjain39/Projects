//package Maze;

import java.awt.Image;

import javax.swing.ImageIcon;

public class Player {

	//tile are for collision
	private int tileX, tileY; //villainX, villainY;
	private Image player, playerFinished, villian;

	public Player(){
		//sets up player image
		ImageIcon img = new ImageIcon("hero.gif");
		player = img.getImage();
		ImageIcon img2 = new ImageIcon("winner.gif");
		playerFinished = img2.getImage();

		//tile starting point
		tileX = 3;
		tileY = 1;
		//villainX = 7;
		//villainY = 4;

	}//end of Player constructor

	//get methods
	public Image getPlayer(){return player;}//end of getPlayer
	public Image getPlayerFinished(){return playerFinished;}//end of getPlayerFinished
//	public Image getVillain(){return villian;}//end of get villain
	public int getTileX(){return tileX;}
	public int getTileY(){return tileY;}
	//public int getVillainX(){return villainX;}
	//public int getVillainY(){return villainY;}

	public void resetPlayer(){
		tileX=1;
		tileY=1;
		
	}
	public void move(int dx, int dy){
		tileX += dx;
		tileY += dy;
	}//end of move 
}