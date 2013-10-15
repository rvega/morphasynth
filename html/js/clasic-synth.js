/*
 * Manages the clasic synth
 */
Morphasynth.ClasicSynth = function(){

  this.synthCanvas = null;

  this.init = function (){

    var self = this;

    var cs = $("#clasic-synth");
    this.synthCanvas = Raphael('clasic-synth', cs.width(), cs.height());

    //stop browser scrolling
    $(cs).bind('touchmove', function(e){e.preventDefault()});

    //set Background
    var bg = this.synthCanvas.rect(0,0,cs.width(),cs.height());
    bg.attr("fill", "#002129");
    self.BackgroundBeauties(cs.width(), cs.height());

    self.drawOnConfig(this.synthCanvas);

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

  this.drawOnConfig = function (paper){
    var title = paper.text(100, 20, "Clasic Synth");
    title.attr("fill","#fff");
    title.attr("font-size","25");
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