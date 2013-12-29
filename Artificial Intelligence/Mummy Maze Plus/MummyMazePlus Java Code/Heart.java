//package Maze;

import java.awt.Image;

import javax.swing.ImageIcon;

public class Heart {
	private Image heart;
	private Image wallReplaceHeart;

	public Heart(){
		ImageIcon img4 = new ImageIcon("heart.gif");
		heart = img4.getImage();
		ImageIcon img5 = new ImageIcon("wall1.png");
		wallReplaceHeart = img5.getImage();
	}
	
	public Image getHeart(){return heart;}//end of get villain

	public void makeWall(){
		heart = wallReplaceHeart;
	}
}