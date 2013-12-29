import java.io.*;
import java.awt.*;
import java.awt.event.*;

class Editor extends WindowAdapter implements ActionListener,TextListener
{
	Frame f,f1,f2;
	Panel p1,p2;
	MenuBar mb;
	Menu m1,m2;
	MenuItem nw,opn,sv,svs,ext,fnd,fndrplc;
	TextArea ta;
	Label l1,l2;
	TextField t1,t2;
	Button b1,b2,b3,b4;
	Boolean naya=false;
	String s,str,fstr,rstr,temp="";
	int pos,npos=0;
	FileInputStream fis;	FileOutputStream fos;    	 
	FileDialog fd1=new FileDialog(f,"Open File",FileDialog.LOAD);
	FileDialog fd2=new FileDialog(f,"Save File",FileDialog.SAVE);
	public Editor()
	{
		f=new Frame("Editor");	f.setSize(600,400);
		ta=new TextArea();
		mb=new MenuBar();
		m1=new Menu("File");	m2=new Menu("Tools");
		nw=new MenuItem("New");		opn=new MenuItem("Open");	sv=new MenuItem("Save");	svs=new MenuItem("Save As");	ext=new MenuItem("Exit");	
		fnd=new MenuItem("Find");	fndrplc=new MenuItem("Find & Replace");
		ta.addTextListener(this);
		nw.addActionListener(this);
		opn.addActionListener(this);
		sv.addActionListener(this);
		svs.addActionListener(this);
		ext.addActionListener(this);
		fnd.addActionListener(this);
		fndrplc.addActionListener(this);
		m1.add(nw);		m1.add(opn);	m1.add(sv);		m1.add(svs);	m1.addSeparator();	m1.add(ext);
		m2.add(fnd);	m2.add(fndrplc);
		mb.add(m1);		mb.add(m2);
		f.setMenuBar(mb); f.add(ta); f.setVisible(true);
		f.addWindowListener(this);	
	}
	
	public void actionPerformed(ActionEvent e)
	{
		String str=e.getActionCommand();
		try
		{
		if(str.equals("New"))
		{
			ta.setText("");		naya=true;
		}
		else
			if(str.equals("Open"))
			{
				fd1.setVisible(true);
				fis=new FileInputStream(fd1.getDirectory()+"/"+fd1.getFile());
				int ch;		String s=new String("");
				while((ch=fis.read())!=-1)
					s=s+(char)ch;
				ta.setText(s);
				fis.close();				
			}
			else
				if(str.equals("Find"))
       			{	
       		    	f1=new Frame();
    				f1.setSize(300,100);
    				f1.setLayout(new FlowLayout());
    				l1=new Label("Find :  ");
    				t1=new TextField(20);
    				b1=new Button("Find Next");
    				b2=new Button("Cancel");
    				f1.add(l1);	f1.add(t1);	f1.add(b1);	f1.add(b2);
    				b1.addActionListener(this);	b2.addActionListener(this);
    				f1.setVisible(true);	MyListener ml=new MyListener();		f1.addWindowListener(ml);
       			}
       			else
       				if(str.equals("Save"))
       				{
       					
       					if(fd1.getFile()==null || naya==true)
       					{
       						fd2.setVisible(true);
       						fos=new FileOutputStream(fd2.getDirectory()+"/"+fd2.getFile());
             				char arr[]=new char[5000];
             				s=ta.getText();   arr=s.toCharArray(); 
             				for(int i=0;i<arr.length;i++)
             				{	
                 				fos.write(arr[i]);
             				}	   
                			fos.close();		
       					}
       					else
       					{
       						fos=new FileOutputStream(fd1.getDirectory()+"/"+fd1.getFile());
             				char arr[]=new char[5000];
             				s=ta.getText();   arr=s.toCharArray(); 
             				for(int i=0;i<arr.length;i++)
             				{	
                 				fos.write(arr[i]);
             				}	   
                			fos.close();	
       					}
       				}		
       				else
       					if(str.equals("Save As"))
       					{
       						fd2.setVisible(true);
       						File f=new File(fd2.getDirectory()+"/"+fd2.getFile());	
             				fos=new FileOutputStream(f);
             				char arr[]=new char[5000];
             				s=ta.getText();   arr=s.toCharArray(); 
             				for(int i=0;i<arr.length;i++)
             				{	
                 			fos.write(arr[i]);
             				}	   
                			fos.close();
       					}
       					else
       						if(str.equals("Exit"))
        					{
            					f.setVisible(false);
            					f.dispose();
            					System.exit(1);
        					}
        					else
        						if(str.equals("Find & Replace"))
       							{
       							f2=new Frame();		f2.setSize(300,200);
    							f2.setLayout(new BorderLayout());
								p1=new Panel();		p1.setLayout(new GridLayout(4,1));
								p2=new Panel();		p2.setLayout(new FlowLayout());    
    							l1=new Label("Find :  ");   l2=new Label("Replace with:  "); 
    							t1=new TextField(20);  t2=new TextField(20);
    							b1=new Button("Find Next");		b2=new Button("Replace");	b3=new Button("Replace All");
    							b4=new Button("Close");    									
         						p1.add(l1);	 p1.add(t1);	p1.add(l2);		p1.add(t2);
         						p2.add(b1);		p2.add(b2);		p2.add(b3);
         						p2.add(b4);   								
    							b1.addActionListener(this);	b2.addActionListener(this);	b3.addActionListener(this);
    							b4.addActionListener(this);   							
    							f2.add(p1,"Center");	f2.add(p2,"South");
    							f2.setVisible(true);	MyListener ml=new MyListener();		f2.addWindowListener(ml);
    		   					}
    		   					else
    		   						if(str.equals("Find Next"))
        							{        							
        							str=ta.getText();
        							str=str.replace("\n", "");        							
        							fstr=t1.getText();
        							if(!temp.equals(fstr))
        								npos=0;
        							temp=new String(fstr);
        							pos=str.indexOf(fstr,npos);
        							if(pos==-1)
        							{
        								npos=0;
        								Frame f3=new Frame();		f3.setSize(500,100);		f3.setLayout(new FlowLayout(FlowLayout.CENTER));
        								Label l=new Label(fstr+" : WORD NOT FOUND ANYMORE & STARTING THE SEARCH AGAIN");
        								f3.add(l);
        								f3.setVisible(true); 	MyListener ml=new MyListener();		f3.addWindowListener(ml);
        							}
        							else
        							{
        								npos=pos+(fstr.length());
        								ta.select(pos,npos);	f.toFront();
        							}
        							}
        							else
        								if(str.equals("Replace"))
        								{     
        									rstr=t2.getText();
        									if(pos
        										==npos)
        									{
        										Frame f3=new Frame();		f3.setSize(500,100);		f3.setLayout(new FlowLayout(FlowLayout.CENTER));
        										Label l=new Label("NO WORD SELECTED");
        										f3.add(l);
        										f3.setVisible(true); 	MyListener ml=new MyListener();		f3.addWindowListener(ml);
        									}
        									else
        									if(pos==-1)
        									{
        										npos=0;
        										Frame f3=new Frame();		f3.setSize(500,100);		f3.setLayout(new FlowLayout(FlowLayout.CENTER));
        										Label l=new Label(fstr+" : WORD NOT FOUND ANYMORE & STARTING THE SEARCH AGAIN");
        										f3.add(l);
        										f3.setVisible(true); 	MyListener ml=new MyListener();		f3.addWindowListener(ml);
        									}
        									else       								
        									{
        										ta.replaceText(rstr,pos,npos);	f.toFront();       									
        									}
        								}
        								
        								else
        									if(str.equals("Replace All"))
        									{
        										str=ta.getText();
        										str=str.replace("\n", "");
        										fstr=t1.getText();
        										rstr=t2.getText();
        										npos=0;		pos=0;
        										while(pos!=-1)
        										{
        										pos=str.indexOf(fstr,npos);
        										npos=pos+(fstr.length());
        										ta.replaceText(rstr,pos,npos);
        										str=ta.getText();
        										str=str.replace("\n", "");
        										pos=str.indexOf(fstr,npos);        										
        										f.toFront();
        										}
        									}
        									else
        										if(str.equals("Close"))
    											{
        											f2.setVisible(false);
        											f2.dispose();
    											}
    											else
    												if(str.equals("Cancel"))
    												{
        												f1.setVisible(false);
        												f1.dispose();
    												}
		}
	catch(IOException e1)
           {
           	e1.getMessage();
           } 
	}

	public void textValueChanged(TextEvent e1)
	{}
	
	public void windowClosing(WindowEvent w)
	{
  		Window win=w.getWindow();
  		win.setVisible(false);
  		win.dispose();
    	System.exit(1);
	}

	public static void main(String args[]) 
	{
		Editor ed=new Editor();
	}
}

class MyListener extends WindowAdapter
{
	public void windowClosing(WindowEvent w)
	{
  		Window win=w.getWindow();
  		win.setVisible(false);
  		win.dispose();
    }
}