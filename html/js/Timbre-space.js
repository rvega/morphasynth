/*
 * Manages the 2D space
 */
Morphasynth.TimbreSpace = function(){

  this.presets = null;
  this.canvas = null;
  this.height = 0;
  this.width = 0;
  this.pointer1;
  this.pointer2;
  this.pointer3;
  this.pointer4;
  this.bglines;
  this.presetDots= new Array();
  this.pointerX;
  this.pointerY;
  this.sendXY = false;
  var originX, originY;
  var offsetX, offsetY; // cantidad de desplazamiento absoluto de el space en un zoom.
  var displacementX, displacementY;// valores de verdacidad temporal para cálculos estáticos de el desplazamiento realizado.
  var zoom, zm;
  var yMiddle;
  var pinching = false;
  var centro;

  //Morphing

  //var pathsArray;
  var currentPath; 
  var pathData;
  var path = null;

  var self = this;

  this.init = function(){
    // Load presets
    this.presets = JSON.parse(ContainerApp.getPresets());

    //stop browser scrolling
    $('#timbre-space').bind('touchmove', function(e){e.preventDefault()});

    // Create drawing surface
    var ts = $('#timbre-space');
    this.width = ts.width();
    this.height = ts.height();
    this.canvas = Raphael('timbre-space', ts.width(), ts.height());
    path = this.canvas.path("M0,0L-10,10");

    //set screenVariables
    yMiddle = ((this.height-68)/2)+68;
    originX = this.width/2;
    originY = yMiddle;
    displacementX = 0;
    displacementY = 0;  
    console.log("origin: "+originX+", "+originY);
    zoom = 1;
    zm = 1;

    //set Background
    var bg = this.canvas.rect(0,0,ts.width(),ts.height());
    bg.attr("fill", "#0A0420");
    this.BackgroundBeauties(ts.width(),ts.height());

    //draw the pointer
    this.pointerX = this.width/2;
    this.pointerY = yMiddle;
    self.drawPointer();

    // Draw the points
    this.drawPoints();

    this.drawUpperMenu();

    //The Touch with hammer.js
    //boolean for single drag
    var singleDrag = false;

    //when touch starts
    Hammer(ts).on("dragstart", function (event) {
        console.log("start: "+event.gesture.touches.length);
        if(event.gesture.touches.length == 1){
          singleDrag = true;

          //begin path
          pathData = new Array();
          currentPath = new Array();
          path.remove();
        }
    });

    //when touch Ends
    Hammer(ts).on("dragend", function (event) {
        self.sendPoses(0);
        console.log("end");
        singleDrag = false;

        if(zoom>1){
          zm = zoom;
        }else{
          zoom = 1;
          zm = zoom;
          originX = self.width/2;
          originY = yMiddle;
          self.actPoints();
        }
    });

    //when is dragging
    Hammer(ts).on("drag", function (event) {

      if(event.gesture.touches.length == 1 && !pinching){

        self.pointerX = event.gesture.touches[0].pageX;
        self.pointerY = event.gesture.touches[0].pageY;

        self.sendPoses(1);
        self.actualicePointer(self.pointerX,self.pointerY);

        //add to path
        //var lx = originX+(zoom*(self.pointerX-(self.width/2)));
        //var ly = originY+(zoom*(self.pointerY-((self.height+68)/2)));

        //logic aproach
        //var lx = (self.pointerX-(self.width/2));
        //var ly = (self.pointerY-((self.height+68)/2));

        //math aproach
        var lx = ((self.pointerX-(self.width/2))/zoom)-offsetX;
        var ly = ((self.pointerY-((self.height+68)/2))/zoom)-offsetY;

        console.log("dat: "+lx+", "+ly);

        if (pathData.length == 0) {
          //no before
          //originX+(zoom*(preset[xDescriptor]*this.width))
          currentPath[0] = [lx, ly];
          pathData[0] = ["M", self.pointerX, self.pointerY];
            //pathData[0] = ["M",self.pointerX,self.pointerY];
            path = self.canvas.path(pathData);
            path.attr({stroke: "#aaa","stroke-width": 2});
        }
        else{
          //pathData[pathData.length] =["L",self.pointerX,self.pointerY];
          pathData[pathData.length] =["L",self.pointerX, self.pointerY];
          currentPath[currentPath.length] = [lx, ly];
        }
        path.attr({path: pathData});
      }
    });

    //touchSwipe Implementation
/*    $(ts).swipe( {
        swipeLeft:function(event, direction, distance, duration, fingerCount) {
          if(fingerCount == 3){
            $("#timbre-space").slideToggle();
            $("#config-panel").slideToggle();
            $("#piano-scroll").slideToggle();
            $("#options-button").slideToggle();
          }
        } ,
        fingers:$.fn.swipe.fingers.ALL
      });*/

      $(ts).swipe( {
        pinchIn:function(event, direction, distance, duration, fingerCount, pinchZoom)
        {
          //$(this).text("You pinched " +direction + " by " + distance +"px, zoom scale is "+pinchZoom);
          self.pinchRelease();
        },
        pinchOut:function(event, direction, distance, duration, fingerCount, pinchZoom)
        {
          //$(this).text("You pinched " +direction + " by " + distance +"px, zoom scale is "+pinchZoom);
          self.pinchRelease();
        },
        pinchStatus:function(event, phase, direction, distance , duration , fingerCount, pinchZoom) {
          //$(this).html("Pinch zoom scale "+pinchZoom+"  <br/>Distance pinched "+distance+" <br/>Direction " + direction);
          if(fingerCount ==2){

            zoom = zm*pinchZoom;

            try{
              if(event.touches.length>1){
                centro = self.middlePoint(event.touches[0].pageX, event.touches[0].pageY, event.touches[1].pageX, event.touches[1].pageY);
                
                if(pinching==false){
                  //set the position
                  displacementX = centro[0]-originX;
                  displacementY = centro[1]-originY;

                  // first frame pinching
                  pinching=true;
                }

                //set the origin with displacement
                originX = centro[0]-displacementX;
                originY = centro[1]-displacementY;

                //set the offset of the timbre space
                offsetX = originX - (self.width/2);
                offsetY = originY - (((self.height-68)/2)+68);
              }
            }
            catch(err){
              alert(err);
            }

             self.zoomPoints();
             self.actPath();

          }
        },
        fingers:2,  
        pinchThreshold:0  
      });


    $(".icon-ok").mouseup(function (){
      self.actPoints();
    }); 

  };

  //SEND CANVAS POSES
  this.sendPoses = function (pres){
    //console.log("pointer poses x:"+xx+" y:"+yy);
    ContainerApp.sendMessage("/pointer/",this.pointerX,this.pointerY,pres);
  }

  this.pinchRelease = function (){
    if(zoom>1){
      zm = zoom;

      //control canvas go out
      if((originX+(zoom*(0.5*self.width)))<self.width) {
        //alert("asdfasdfasdf");
        originX = self.width-((zoom*self.width)/2);
      }
      if((originX+(zoom*(-0.5*self.width)))>0) {
        originX = ((zoom*self.width)/2);
      }
      //control canvas go out
      if((originY+(zoom*(0.5*self.height)))<self.height) {
        //alert("asdfasdfasdf");
        originY = self.height-((zoom*self.height)/2)+68;
      }
      if((originY+(zoom*(-0.5*self.height)))>0) {
        //alert("asdfasdfasdf");
        originY = ((zoom*self.height)/2);
      }

      self.actPoints();
      self.actPath();
    }else{
      zoom = 1;
      zm = zoom;
      originX = self.width/2;
      originY = yMiddle;
      self.actPoints();
      self.actPath();
    }
    pinching = false;
  }

  this.drawPoints = function(){
    var preset, x, y, xDescriptor, yDescriptor;

    for(var i=0; i<this.presets.length; i++) {
      preset = this.presets[i];
      xDescriptor = Options.x;
      yDescriptor = Options.y;
      x =originX+(zoom*(preset[xDescriptor]*this.width));
      y = originY+(zoom*((this.height-68)*preset[yDescriptor]));
      this.presetDots[i] = this.canvas.circle(x, y, 2);
      this.presetDots[i].attr("fill", "#FFF");
      this.presetDots[i].attr("stroke-width",0);
      //console.log("preset: "+preset[xDescriptor]+" ,  "+preset[yDescriptor]);
    }
  };

  this.actPoints = function(){
    var preset, x, y, xDescriptor, yDescriptor;

    for(var i=0; i<this.presets.length; i++) {
      preset = this.presets[i];
      xDescriptor = Options.x;
      yDescriptor = Options.y;
      x =originX+(zoom*(preset[xDescriptor]*this.width));
      y = originY+(zoom*((this.height-68)*preset[yDescriptor]));
      this.presetDots[i].animate({'cx': x,'cy':y},1500,'ease-in-out');
      //console.log("preset: "+preset[xDescriptor]+" ,  "+preset[yDescriptor]);
    }
  };

  this.zoomPoints = function(){
    var preset, x, y, xDescriptor, yDescriptor;

    for(var i=0; i<this.presets.length; i++) {
      preset = this.presets[i];
      xDescriptor = Options.x;
      yDescriptor = Options.y;
      x =originX+(zoom*(preset[xDescriptor]*this.width));
      y = originY+(zoom*((this.height-68)*preset[yDescriptor]));
      //this.presetDots[i].animate({'cx': x,'cy':y},1500,'ease-in-out');
      this.presetDots[i].attr({'cx': x,'cy':y});
      //console.log("preset: "+preset[xDescriptor]+" ,  "+preset[yDescriptor]);
    }
  };

  this.actPath = function (){
    //pathData[0] = ["M", lx, ly];
    for (var i = 0; i < currentPath.length; i++) {
      var tx =originX+(zoom*(currentPath[i][0]));
      var ty = originY+(zoom*(currentPath[i][1]));
      // var tx = (zoom*currentPath[i][0]);
      // var ty = (zoom*currentPath[i][1]);
      pathData[i] = [pathData[i][0], tx, ty];
    };
    path.attr({path: pathData});
  }

  this.drawPointer = function (){
    console.log("pointerX: "+this.pointerX+"  pointerY:"+this.pointerY);

    this.pointer1=this.canvas.circle(this.pointerX,this.pointerY,18);
    this.pointer1.attr("fill-opacity","0");
    this.pointer1.attr("stroke-width","1");
    this.pointer1.attr("stroke-opacity",".3");
    this.pointer1.attr("stroke","#fff");

    this.pointer2=this.canvas.circle(this.pointerX,this.pointerY,25);
    this.pointer2.attr("fill-opacity","0");
    this.pointer2.attr("stroke-width","2");
    this.pointer2.attr("stroke","#fff")

/*    this.pointer3=this.canvas.circle(this.pointerX,this.pointerY,33);
    this.pointer3.attr("fill-opacity","0");
    this.pointer3.attr("stroke-width","6");
    this.pointer3.attr("stroke","#fff");
    this.pointer3.attr("stroke-dasharray","--.");
    this.pointer3.attr("stroke-linecap","square");*/
  };

  this.actualicePointer = function (xx,yy){
    this.pointer1.attr("cx",xx);
    this.pointer1.attr("cy",yy);
    this.pointer2.attr("cx",xx);
    this.pointer2.attr("cy",yy);
/*    this.pointer3.attr("cx",xx);
    this.pointer3.attr("cy",yy);*/
  };

  this.BackgroundBeauties = function (wid, hei){
    //Dots
    for(var i=15;i<wid;i+=30){
      for(var j=15;j<hei;j+=30){
        var pointer=this.canvas.circle(i,j,.3);
        pointer.attr({"fill":"#aaa","stroke-width":"0"});
      }
    }

    //Under canvas Lines
/*    for(var i=0;i<(wid+100);i+=12){
      bglines=this.canvas.path("M"+i+",712L"+(i-100)+",780");
      bglines.attr("stroke","#fff");
      bglines.attr("stroke-width","1");
      bglines.attr("stroke-opacity",".3");
    }*/
  };

  this.drawUpperMenu = function (){
    this.canvas.rect(0,0,this.canvas.width, 68).attr({'fill':'#22303D', 'stroke-width':'0'});
  };

  this.middlePoint = function (x1, y1, x2, y2) {
    return [(x1+x2)/2,(y1+y2)/2];
  };

}