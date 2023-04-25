//pressing carousel for DYMO label tabe

/*maybe it come in handy to calculate the deflection of the tongues (beams)
 starting point: https://mechanicalc.com/reference/beam-deflection-tables
  
  Deflection: δ = -(Fx²/6EI)*(3L-x), δmax=FL³/3EI @x=L
  Slope: Θ = -(Fx / 2EI)*(2L-x), Θmax = (FL² / 2EI) @x=L
  
  L,x: length, position [m]
  F: Force at l [N]
  E: youngs modulus [Pa]
  I: Moment of Inertia [m^4]
  
  Isquare= w*h³/12 |square([w,h]) (deflection in around y)
  
*/

/*
  About symbols and fonts
  for portability you should use the fonts embedded with openSCAD (LiberationSans,..Mono,..Serif, see Help->Fontlist)
  you can use some Unicode Characters as well by either copying them from a website or using the codes like \u263A (Smiley hex)
  e.g. https://www.vertex42.com/ExcelTips/unicode-symbols.html
  Attention! most of them will not work
*/

/* [Parameters] */
$fn=50;
//charset
charset= "custom"; //["custom","16segment"]
cstmChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ-.1234567890X";
//general spacing between all parts
spcng=0.2; //spacing between parts

/* [Emboss] */
//textheight above baseline
txtSize=2.5; 
//X-Y spacing between positive and negative
embXYSpcng=0.15; 
//Z spacing between positive and negative
embZSpcng=0.15; 
//embossing height
embHght=0.5; 

/* [Top Disc] */
// render a label with the characterset
rndrTopLabel=true;
//outer Dia of top disc (negative)
topDscDia=55.9; 
//Thickness of top disc
topDscThck=3;
//Center bore diameter of top disc
topCntrDia=10.8;
//Diameter of the detends
dtntDia=2;

/* [Bottom Disc] */
//outer Dia 
botDscDia=51.3; 
//thickness
botDscThck=1.2;  
//zOffset of tongues from bottom
botZOffset=2.15; 
//Diameter of the chars baseline
bsLneDia=49.2; 
//width of the slots between tongs
sltWdth=0.6; 

//Diameter of nonslotted part
sltCntrDia=22; 
//Diameter of center cylinder
botCntrDia=13.95;
//Total height of center cylinder
botCntrHght=5.2; 
//Diameter of the center bore
cntrBore=7.1; 

/* [show] */
showTopDsc = true;
showBotDsc = true;

/* [Tune] */
//apply a sectionCut 
showSection= false;
//select char to cut, 0: anvil
wheelRot=0;
//tilt the wheels to have the tip of tongue lay flat
wheelTilt= false;
showCharInTopDsc=false;

/* [Hidden] */
charTilt=8.9;
fudge=0.1;
//guide dimensions
gdWdth=2;


// -- charsets --
//16 segement LCD inspired charset, each segment alone plus some common combinations, can be combined to capital, lowercase and numbers
segmented16=[["a1"],["a2"],["b"],["c"],["d1"],["d2"],["e"],["f"],["g1"],["g2"],["h"], ["i"],["j"], ["k"], ["l"],["m"],
             ["a1","a2"],["d1","d2"],["g1","g2"],["j","k"],["a1","a2","f","e"],["d1","d2","b","c"],["h","m"],["a1","a2","b","c","d1","d2","e","f"]];

chars = (charset=="custom") ? cstmChars : segmented16;
//a polygon to revolute into the disc shape
charCount=len(chars); 
dscPoly=[[botCntrDia/2-fudge,2.15],[sltCntrDia/2,2.15],[37.6/2,2],[38.6/2,3],[botDscDia/2,2], //bottom face
         [botDscDia/2,2+botDscThck],[38.6/2-0.5,3+botDscThck],[37.6/2-0.5,2+botDscThck],[sltCntrDia/2,2.15+botDscThck],[botCntrDia/2-fudge,2.15+botDscThck]];
tngAng=360/(charCount+1); //angle of one tongue

lngGuide=botCntrDia+3.3;
shrtGuide=botCntrDia;
sectZRot = wheelRot ? wheelRot*tngAng : 0;
sectYTilt = wheelTilt ? -charTilt : 0;

if (showTopDsc){
  rotate([0,sectYTilt,0]) difference(){
    rotate(sectZRot) topDsc();
    if (showSection) color("darkred")
      translate([0,-(topDscDia/2+fudge)/2,botCntrHght+topDscThck/2]) 
        cube([topDscDia+fudge,topDscDia/2+fudge,topDscThck+fudge],true);
    }
}
if (showBotDsc) 
  rotate([0,sectYTilt,0]) difference(){
    rotate(sectZRot) botDsc();
    if (showSection) color("darkred")
        translate([0,-(botDscDia/2+fudge)/2,(botCntrHght+topDscThck)/2]) 
          cube([botDscDia+fudge,botDscDia/2+fudge,botCntrHght+topDscThck+fudge],true);
    }
//tuning: show the chars as if they where pressed into the top disc    
if (showCharInTopDsc)
  rotate([0,sectYTilt,0]) difference(){
    color("ivory") for (i=[0:len(chars)-1])
      rotate(tngAng*i+tngAng+sectZRot) translate([bsLneDia/2,0,botCntrHght]) 
        rotate([0,0,90]) linear_extrude(embHght,convexity=3) 
          text(chars[i],size=txtSize, halign="center");
    if (showSection) color("darkred")
      translate([0,-(topDscDia/2+fudge)/2,botCntrHght+topDscThck/2]) 
        cube([topDscDia+fudge,topDscDia/2+fudge,topDscThck+fudge],true);
  }

//debug tongue cross section
*polygon(dscPoly);

module topDsc(){
  lblThck=0.2;
  //top disc with negatives
  color("darkSlateGrey") translate([0,0,botCntrHght]) 
    difference(){
      linear_extrude(topDscThck,convexity=3)
        difference(){ //2D difference
          circle(d=topDscDia,$fn=charCount+1);
          //center
          circle(d=topCntrDia);
          //detents
          for (ang=[0:tngAng:360-tngAng]) 
            rotate(ang) translate([topDscDia/2,0,0]) circle(d=dtntDia);
          //guides
          hull() for (ix=[0,1])
            translate([ix*(shrtGuide-gdWdth)/2,0])
              circle(d=gdWdth+spcng);
          hull() for (iy=[0,1])
            translate([0,iy*(lngGuide-gdWdth)/2])
              circle(d=gdWdth+spcng);
        }
      //groove for labels
      if (rndrTopLabel)
        rotate_extrude() translate([bsLneDia/2-txtSize*1.25,topDscThck-lblThck,0]) 
          square([txtSize*1.5,lblThck+fudge]);
          
      //chars 
      for (i=[0:len(chars)-1])
          rotate(tngAng*i+tngAng) translate([bsLneDia/2,0,-fudge]) 
            rotate([0,0,90]) linear_extrude(embHght+embZSpcng+fudge) 
            if(charset=="custom")
              offset(embXYSpcng) text(chars[i],size=txtSize, halign="center");
            else if (charset=="16segment")
              offset (embXYSpcng) sixteenSegment(size=2.5, draw=segmented16[i]); 
              
    }
    //labels
    color("yellow") if (rndrTopLabel){
        for (i=[0:len(chars)-1])
          rotate(tngAng*i+tngAng) translate([bsLneDia/2,0,topDscThck+5]) 
            rotate([0,0,90]) linear_extrude(lblThck) 
            if(charset=="custom")
              text(chars[i],size=txtSize, halign="center");
            else if(charset=="16segment")
              sixteenSegment(size=2.5, draw=segmented16[i]);
    }
}

module botDsc(){
  //disc with tongues and positives
  
  //center dome
  color("ivory") rotate(-tngAng/2) linear_extrude(botCntrHght) difference(){
    circle(d=botCntrDia,$fn=charCount+1);
    circle(d=cntrBore);
  }
  //guides in topDsc
  color("ivory") translate([0,0,botCntrHght]) rotate(-tngAng/2) difference(){
    union(){
      cylinder(d=topCntrDia-fudge*2,h=topDscThck,$fn=charCount+1);  
      //guide bars
      //short
      rotate(tngAng/2){
        hull() for (ix=[0,1])
          translate([ix*(botCntrDia-gdWdth)/2,0,0]){
            cylinder(d=gdWdth,h=topDscThck-gdWdth/2);
            translate([0,0,topDscThck-gdWdth/2]) sphere(d=gdWdth);
          }
        //long
        hull() for (iy=[0,1])
          translate([0,iy*(lngGuide-gdWdth)/2,0]){
            translate([0,0,-(botCntrHght-botZOffset)]) 
              cylinder(d=gdWdth,h=topDscThck-gdWdth/2+(botCntrHght-botZOffset));
            translate([0,0,topDscThck-gdWdth/2]) sphere(d=gdWdth);
          }
        }
      }
      translate([0,0,-botCntrHght-fudge/2]) cylinder(d=cntrBore,h=topDscThck+botCntrHght+fudge);
    }
  
  //rotate extrude the cross section and cut slots
  color("ivory") rotate(-tngAng/2) difference(){
    rotate_extrude($fn=50/*charCount+1*/) polygon(dscPoly);
    //slots
     for (ang=[0:tngAng:360-tngAng]){
      sltLngth=(ang<tngAng*2) ? (botDscDia-sltCntrDia)/2-3 : (botDscDia-sltCntrDia)/2;
      sltOffset = (ang<tngAng*2) ? 3 : 0;
      rotate(ang) translate([(sltCntrDia+sltLngth+fudge)/2+sltOffset,0,botCntrHght/2]) 
        cube([sltLngth+fudge,sltWdth,botCntrHght-fudge],true);
    }
  }
  //anvil
  color("ivory") translate([botDscDia/2+1.4,0,3]) 
    rotate([0,charTilt,0]) translate([-8.3,-1,-botDscThck]) 
      cube([8.3,2,botDscThck+0.7]);
      
  //chars
  color("ivory") for (i=[0:len(chars)-1])
    rotate(tngAng*i+tngAng) translate([bsLneDia/2,0,3.4-0.2]) // z-Offset to connect chars to tongue tipp securely
      rotate([charTilt,0,90]) 
        linear_extrude(embHght+0.14, convexity=3) //compensate for z-Offset of chars
        if(charset=="custom")  
          text(chars[i],size=txtSize, halign="center");
        else if(charset=="16segment")
          sixteenSegment(size=2.5, draw=segmented16[i]);
        
}

module sixteenSegment(size=2.5, draw=["a1"]){
  //can give in draw a list of segments to draw, if nothing is given all segments are drawn
  /*
        a1  a2
       ---i--- 
      |\h | j/|
     f| \ | / |b
      |  \|/  |
     g1--- ---g2
      |  /|\  |
     e| / | \ |c
      |/k | m\|
       ---l---
       d1   d2
  */
  // https://en.wikipedia.org/wiki/Sixteen-segment_display
  stroke=size/10;
  gap=0.03;
  gap45=gap/(1/2*sqrt(2));
  ovWdth=size/2;
  ovHght=size*0.95; //size correction
  
  //text for size and positioning reference
  *text("8",halign="center",size=2.5);
  
  segments=[["a1"],["a2"],["b"],["c"],["d1"],["d2"],["e"],["f"],["g1"],["g2"],["h"], ["i"],["j"], ["k"], ["l"],["m"]];
  segType= ["h", "h", "v","v","h", "h", "v","v","h", "h", "dd","v","du","du","v","dd"]; //h,v,dd,du (diagonal down, up)
  segPos=[
          [-ovWdth/4,ovHght],//a1
          [ovWdth/4,ovHght],//a2
          [ovWdth/2,ovHght*3/4], //b
          [ovWdth/2,ovHght/4], //c
          [-ovWdth/4,0],//d1
          [ovWdth/4,0],//d2
          [-ovWdth/2,ovHght/4], //e
          [-ovWdth/2,ovHght*3/4], //f
          [-ovWdth/4,ovHght/2],//g1
          [ovWdth/4,ovHght/2],//g2  
          [-ovWdth/4,ovHght*3/4],//h
          [0,ovHght*3/4],//i  
          [ovWdth/4,ovHght*3/4],//j
          [-ovWdth/4,ovHght/4],//k
          [0,ovHght/4],//l
          [ovWdth/4,ovHght/4],//m
          ];
  
  segID=search(draw,segments);
  //draw all segments
  selection= (len(segID)==0) ? [0:len(segments)] :  segID;
  for (i=selection){
    if (segType[i]=="h")
      translate(segPos[i]) horizontal();
    if (segType[i]=="v")
      translate(segPos[i]) vertical();
    if (segType[i]=="du")
      translate(segPos[i]) diagonal(true);
    if (segType[i]=="dd")
      translate(segPos[i]) diagonal(false);
  }

  module vertical(){
    hull() for (iy=[-1,1])
      translate([0,iy*(ovHght/2-stroke-gap45)/2]) circle(d=stroke, $fn=4);
  }
  module horizontal(){
    hull() for (ix=[-1,1])
      translate([ix*(ovWdth/2-stroke-gap45)/2,0]) circle(d=stroke, $fn=4);
  }
  
  module diagonal(isUp=true){
    poly=[
          [-(ovWdth-stroke)/2+gap,(ovHght-stroke)/2-gap],
          [-stroke/2-gap,stroke*2+gap],
          [-stroke/2-gap,stroke/2+gap],
          [-(ovWdth-stroke)/2+gap,ovHght/2-stroke*2-gap]
    ];
    im = isUp ? [1,0,0] : [0,0,0] ;
    mirror(im) translate([ovWdth/4,-ovHght/4]) polygon(poly);
  }
}



