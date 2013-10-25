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
  this.pointerX = $('#timbre-space').width()/2;
  this.pointerY = $('#timbre-space').height()/2;
  this.sendXY=false;

  //Morphing
  var pathData;
  var pathString = "";
  var path = null;

  this.init = function(){
    // Load presets
    this.presets = JSON.parse(ContainerApp.getPresets());

    // Bind mouse events
    var self = this;

    //stop browser scrolling
    $('#timbre-space').bind('touchmove', function(e){e.preventDefault()});

    // Create drawing surface
    var ts = $('#timbre-space');
    this.width = ts.width();
    this.height = ts.height();
    this.canvas = Raphael('timbre-space', ts.width(), ts.height());
    path = this.canvas.path("M0,0L-10,10");


    //set Background
    var bg = this.canvas.rect(0,0,ts.width(),ts.height());
    bg.attr("fill", "#0A0420");
    this.BackgroundBeauties(ts.width(),ts.height());

    //draw the pointer
    self.drawPointer();
    // Draw the points
    this.drawPoints();



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
          path.remove();
          pathString = "M"+event.gesture.touches[0].pageX+","+event.gesture.touches[0].pageY;
        }
    });

    //when touch Ends
    Hammer(ts).on("dragend", function (event) {
        self.sendPoses(0);
        console.log("end");
        singleDrag = false;
    });

    //when is dragging
    Hammer(ts).on("drag", function (event) {

        self.pointerX = event.gesture.touches[0].pageX;
        self.pointerY = event.gesture.touches[0].pageY;

        self.sendPoses(1);
        self.actualicePointer(self.pointerX,self.pointerY);

        //add to path
        //pathData.push({self.pointerX, self.pointerY});
        path.remove();
        pathString += " L"+event.gesture.touches[0].pageX+","+event.gesture.touches[0].pageY;
        path = self.canvas.path(pathString);
        path.attr({'stroke':'#B9E9E1', 'stroke-width':'2', 'stroke-opacity':'.3'});
        //console.log("path: "+pathString);
    });

    //When just make a little tap
    Hammer(ts).on("touch", function (event) {

        if(event.gesture.touches.length == 1){
          self.pointerX = event.gesture.touches[0].pageX;
          self.pointerY = event.gesture.touches[0].pageY;

          self.sendPoses(1);
          self.actualicePointer(self.pointerX,self.pointerY);
        }
    });

    //touchSwipe Implementation
    $(ts).swipe( {
        swipeLeft:function(event, direction, distance, duration, fingerCount) {
          if(fingerCount == 2){
            $("#timbre-space").slideToggle();
            $("#config-panel").slideToggle();
            $("#piano-scroll").slideToggle();
            $("#options-button").slideToggle();
          }
        },
        threshold:200,
        fingers:'all'
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

  this.drawPoints = function(){
    var preset, x, y, xDescriptor, yDescriptor;
    for(var i=0; i<this.presets.length; i++) {
      preset = this.presets[i];
      xDescriptor = Options.x;
      yDescriptor = Options.y;
      x = preset[xDescriptor] * this.width;
      y = (preset[yDescriptor] * (this.height-68))+68;
      this.presetDots[i] = this.canvas.circle(x, y, 2);
      this.presetDots[i].attr("fill", "#FFF");
      this.presetDots[i].attr("stroke-width",0);
    }
  };

  this.actPoints = function(){
    var preset, x, y, xDescriptor, yDescriptor;
    for(var i=0; i<this.presets.length; i++) {
      preset = this.presets[i];
      xDescriptor = Options.x;
      yDescriptor = Options.y;
      x = preset[xDescriptor] * this.width;
      y = preset[yDescriptor] * this.height-68;
      this.presetDots[i].animate({'cx': x,'cy':y},1500,'ease-in-out');
    }
  };

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

    this.pointer3=this.canvas.circle(this.pointerX,this.pointerY,33);
    this.pointer3.attr("fill-opacity","0");
    this.pointer3.attr("stroke-width","6");
    this.pointer3.attr("stroke","#fff");
    this.pointer3.attr("stroke-dasharray","--.");
    this.pointer3.attr("stroke-linecap","square");
  };

  this.actualicePointer = function (xx,yy){
    this.pointer1.attr("cx",xx);
    this.pointer1.attr("cy",yy);
    this.pointer2.attr("cx",xx);
    this.pointer2.attr("cy",yy);
    this.pointer3.attr("cx",xx);
    this.pointer3.attr("cy",yy);
  };

  this.BackgroundBeauties = function (wid, hei){
    //Dots
    for(var i=15;i<wid;i+=30){
      for(var j=15;j<hei;j+=30){
        var pointer=this.canvas.circle(i,j,.3);
        pointer.attr("fill","#666");
        pointer.attr("stroke-width","0");
      }
    }

    this.canvas.rect(0,0,this.canvas.width, 68).attr({'fill':'#22303D', 'stroke-width':'0'});

    //Under canvas Lines
/*    for(var i=0;i<(wid+100);i+=12){
      bglines=this.canvas.path("M"+i+",712L"+(i-100)+",780");
      bglines.attr("stroke","#fff");
      bglines.attr("stroke-width","1");
      bglines.attr("stroke-opacity",".3");
    }*/
  };
}