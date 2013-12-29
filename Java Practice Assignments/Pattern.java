import java.awt.*;
import java.awt.event.*;

class Pattern implements MouseMotionListener 
{
	Frame f;		Graphics g;       Dimension d;
	int x1,y1,x2,y2;
	
	public Pattern()
	{
		f=new Frame();
		f.setSize(400,400);
		f.addMouseMotionListener(this);
		f.setVisible(true);
		
		g=f.getGraphics();
	}
	
	public void mouseDragged(MouseEvent e)
	{
		d=f.getSize();
		x2=d.width;    y2=d.height;
		
		x1=e.getX();			y1=e.getY();
		
		g.setColor(Color.red);
		g.drawLine(0,0,x1,y1);
		
		g.setColor(Color.blue);
		g.drawLine(0,y2,x1,y1);
		
		g.setColor(Color.yellow);
		g.drawLine(x2,0,x1,y1);
		
		g.setColor(Color.green);
		g.drawLine(x2,y2,x1,y1);
	}
	
	public void mouseMoved(MouseEvent e1)
	{
		
		
	}
	
	public static void main(String[] args)
	{
		Pattern p=new Pattern ();
	}
    
    
}