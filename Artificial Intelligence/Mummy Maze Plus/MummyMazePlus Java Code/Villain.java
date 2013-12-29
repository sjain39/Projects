//package Maze;

import java.awt.Image;

import javax.swing.ImageIcon;

public class Villain {
	private int villainX, villainY;
	private Image villian;

	public Villain(){
	ImageIcon img3 = new ImageIcon("mummy4.gif");
	villian = img3.getImage();

	villainX = 17;
	villainY = 17;
	}
	
	public Image getVillain(){return villian;}//end of get villain
	public int getVillainX(){return villainX;}
	public int getVillainY(){return villainY;}

	public void resetVillian(){
		villainX = 17;
		villainY = 17;
	}
	public void villainMove(int dx, int dy){
		villainX += dx;
		villainY += dy;
	}//end of villainMove 

	public void setVillain(int x, int y){
		villainX = x;
		villainY = y;
	}
}