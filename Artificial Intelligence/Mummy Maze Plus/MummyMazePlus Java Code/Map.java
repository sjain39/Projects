//package Maze;
import java.util.*;
import java.awt.*;
import java.io.*;
import javax.swing.*;
public class Map {


	private String[] map = new String[64];
	private String[] score = new String[64];

	private Image grass, wall, ladder, finish, water, welcomeScreen, selectLevel, pausegame, gameover, firepole, pitfall, blood, option, mainscreen, highscores;

	public Map(){
		//defines grass and wall and finish images
		ImageIcon img = new ImageIcon("grass1.png");
		grass = img.getImage();
		img = new ImageIcon("wall1.png");
		wall = img.getImage();
		img = new ImageIcon("flag.gif");
		finish = img.getImage();
		img = new ImageIcon("ladder.gif");
		ladder = img.getImage();
		img = new ImageIcon("water.gif");
		water = img.getImage();
		img = new ImageIcon("welcom.jpg");
		welcomeScreen = img.getImage();
		img = new ImageIcon("level.jpg");
		selectLevel = img.getImage();
		img = new ImageIcon("pausegame.jpg");
		pausegame = img.getImage();
		img = new ImageIcon("gameover.gif");
		gameover = img.getImage();
		img = new ImageIcon("firepole.gif");
		firepole = img.getImage();
		img = new ImageIcon("pit.gif");
		pitfall = img.getImage();
		img = new ImageIcon("blood2.gif");
		blood = img.getImage();
		img = new ImageIcon("option.jpg");
		option = img.getImage();
		img = new ImageIcon("mainscreen.jpg");
		mainscreen = img.getImage();
		img = new ImageIcon("scores.jpg");
		highscores = img.getImage();
		//methods below to open, read and close file
		openFile("Map1-1");
		openScores();
	}
	//get methods
	public Image getGrass(){return grass;}
	public Image getWall(){return wall;}
	public Image getFinish(){return finish;}
	public Image getLadder(){return ladder;}
	public Image getWater(){return water;}
	public Image getWelcomeScreen(){return welcomeScreen;}
	public Image getSelectLevelScreen(){return selectLevel;}
	public Image getFirepole(){return firepole;}
	public Image getGameover(){return gameover;}
	public Image getPausegame(){return pausegame;}
	public Image getPitfall(){return pitfall;}
	public Image getBlood(){return blood;}
	public Image getOption(){return option;}
	public Image getMainScreen(){return mainscreen;}
	public Image getScoreScreen(){return highscores;}
	
	public String getMap(int x, int y){
		StringBuffer index = new StringBuffer();
		index.append(map[y].substring(x, x+1 ));
		return index.toString();	
	}//end of getMap

	public void openFile(String fName){
		try{			
			File file = new File(fName+".txt");
			Scanner scan = new Scanner(file);
			while(scan.hasNext() ){
				for(int i = 0; i < map.length; i++)
					map[i] = scan.next();
			}
			scan.close();
		}
		catch (Exception e){
			System.out.println("Everything is working awesome!");
		}
	}//end of openFile
	
	public String getScores(int y){
		return score[y];
	}//end of getScores
	
	public void openScores(){
		try{			
			File file = new File("highscores.txt");
			Scanner scan = new Scanner(file);
			while(scan.hasNext() ){
				for(int i = 0; i < 20; i++)
					score[i] = scan.next();
			}
			scan.close();
		}
		catch (Exception e){
			System.out.println("Everything is working awesome!");
		}
	}


}//end of class Map