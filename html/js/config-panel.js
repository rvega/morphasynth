/*
 * Manages the config panel
 */
Morphasynth.ConfigPanel = function(){

  this.configCanvas = null;

  this.init = function (){

    var self = this;

    var cp = $("#config-panel");
    this.configCanvas = Raphael('config-panel', cp.width(), cp.height());

    //stop browser scrolling
    $(cp).bind('touchmove', function(e){e.preventDefault()});

    //set Background
    var bg = this.configCanvas.rect(0,0,cp.width(),cp.height());
    bg.attr("fill", "#002129");
    self.BackgroundBeauties(cp.width(), cp.height());

    self.drawOnConfig(this.configCanvas);

    //return to canvas swipe
    $(cp).swipe( {
        swipe:function(event, direction, distance, duration, fingerCount) {

          if(fingerCount == 2){
            if(direction == 'right'){
              $("#timbre-space").slideToggle();
              $("#config-panel").slideToggle();
              $("#piano-scroll").slideToggle();
            }
            if(direction == 'left'){
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
    var title = paper.text(100, 20, "Config Panel");
    title.attr("fill","#fff");
    title.attr("font-size","25");
  };

  this.BackgroundBeauties = function (wid, hei){
    //Dots
    for(var i=15;i<wid;i+=30){
      for(var j=15;j<hei;j+=30){
        var pointer=this.configCanvas.circle(i,j,.3);
        pointer.attr("fill","#666");
        pointer.attr("stroke-width","0");
      }
    }
  };
}