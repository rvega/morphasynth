/*
 * Manages the clasic synth
 */
Morphasynth.ClasicSynth = function(){

  this.synthCanvas = null;
  var cs;
  var currentButton = null;
  var buttons = new Array();

  this.init = function (){

    var self = this;

    cs = $("#clasic-synth");
    this.synthCanvas = Raphael('clasic-synth', cs.width(), cs.height());

    //create buttons
    for(var i=0 ; i<5 ; i++){
      var button = new Morphasynth.Button();
      button.init("papucho "+i, this.synthCanvas);
      buttons.push(button);
    }
    console.log("butones: "+buttons.length);

    //stop browser scrolling
    $(cs).bind('touchmove', function(e){e.preventDefault()});

    //set Background
    var bg = this.synthCanvas.rect(0,0,cs.width(),cs.height());
    bg.attr("fill", "#002129");
    self.BackgroundBeauties(cs.width(), cs.height());

    self.drawOn(this.synthCanvas);

    //return to canvas swipe
    $(cs).swipe( {
        swipe:function(event, direction, distance, duration, fingerCount) {

          if(fingerCount == 2){
            if(direction == 'right'){
              $("#config-panel").slideToggle();
              $("#clasic-synth").slideToggle();
              $("#piano-scroll").slideToggle(); 
            }
          }
        },
        threshold:0,
        fingers:'all'
      });
  };

  this.drawOn = function (paper){
    var title = paper.text(100, 20, "Clasic Synth");
    title.attr("fill","#fff");
    title.attr("font-size","25");

    //put buttons on canvas
    for (var i = buttons.length - 1; i >= 0; i--) {
      buttons[i].draw(70+(i*100), 115);
    };
    
    //when touch starts
    Hammer(cs).on("touch", function (event) {
        console.log("start: "+event.gesture.touches.length);

        if(event.gesture.touches.length == 1){

          for (var i = buttons.length - 1; i >= 0; i--) {
            if(buttons[i].getTouch(event.gesture.touches[0].pageX,event.gesture.touches[0].pageY)){
              currentButton = buttons[i];
              //alert("yaerahh !!  "+currentButton.getName());
            }
          };

        }
    });

    //while touching
    Hammer(cs).on("drag", function (event) {
        //console.log("start: "+event.gesture.touches.length);
        if(event.gesture.touches.length == 1 && currentButton!=null){
          currentButton.setAngle(event.gesture.angle);
        }
    });

    //when touch finishes
    Hammer(cs).on("release", function (event) {
        currentButton = null;
    });

  };

  this.BackgroundBeauties = function (wid, hei){
    //Dots
    for(var i=15;i<wid;i+=30){
      for(var j=15;j<hei;j+=30){
        var pointer=this.synthCanvas.circle(i,j,.3);
        pointer.attr("fill","#666");
        pointer.attr("stroke-width","0");
      }
    }
  };
}