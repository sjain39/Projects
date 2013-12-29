import java.awt.*;
import java.awt.event.*;

class Drawing implements MouseMotionListener
{
	Frame f;
	Graphics g;
	int x1,x2,y1,y2,tmp1=0,tmp2=0;

    public Drawing()
    {
    	f=new Frame();
    	f.setSize(350,350);
    	f.addMouseMotionListener(this);
    	f.setVisible(true);
    	g=f.getGraphics();
    }
    
    public void mouseDragged(MouseEvent e1)
    {
    	x2=e1.getX();	 y2=e1.getY();
    	x1=tmp1;	y1=tmp2;
    	tmp1=x2;	tmp2=y2;
    	g.drawLine(x1,y1,x2,y2);
    }
    
    public void mouseMoved(MouseEvent e2)
    {    	
    	tmp1=e2.getX();		tmp2=e2.getY();
    }
    
    public static void main(String args[])
    {
    	Drawing d=new Drawing();
    }
}