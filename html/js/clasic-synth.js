/*
 * Manages the clasic synth
 */
Morphasynth.ClasicSynth = function(){

  this.synthCanvas = null;
  var cs;
  var currentButton = null;
  var buttons = new Array();
  var butNames;

  this.init = function (ky){

    var self = this;

    butNames = ["osc1-amplitude","osc1-waveform","osc1-finetune","osc2-amplitude","osc2-waveform","osc2-finetune","osc3-amplitude","osc3-waveform","osc3-finetune","lp-freq","lp-resonance","lp-envelope-a","lp-envelope-d","hp-freq","hp-resonance","hp-envelope-a","hp-envelope-d","lp-keyfollow","lp-contour","lp-envelope-s","lp-envelope-r","hp-keyfollow","hp-contour","hp-envelope-s","hp-envelope-r","lp-lfo-freq","lp-lfo-waveform","lp-lfo-amplitude","noise-amplitude","hp-lfo-freq","hp-lfo-waveform","hp-lfo-amplitude","lfo1-freq","lfo1-waveform","lfo2-freq","lfo2-waveform","lfo3-freq","lfo3-waveform","lfo1-to-amplitude","lfo1-to-freq","lfo2-to-amplitude","lfo2-to-freq","lfo3-to-amplitude","lfo3-to-freq","envelope-a","envelope-d","envelope-s","envelope-r"];
    console.log("nams: "+butNames.length);

    cs = $("#clasic-synth");
    this.synthCanvas = Raphael('clasic-synth', cs.width(), cs.height());

    //create buttons
    for(var i=0 ; i<48 ; i++){
      var button = new Morphasynth.Button();
      button.init(""+butNames[i], this.synthCanvas);
      buttons.push(button);
    };
    console.log("butones: "+buttons.length);

    //stop browser scrolling
    $(cs).bind('touchmove', function(e){e.preventDefault()});

    //set Background
    var bg = this.synthCanvas.rect(0,0,cs.width(),cs.height());
    bg.attr("fill", "#0A0420");
    //var bg = this.synthCanvas.image('img/fondo.jpg',0,0,cs.width(),cs.height());
    
    //put background dots
    //self.BackgroundBeauties(cs.width(), cs.height());

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

    //first row
    var cnt = 0;
    for (var i = 0; i <9; i++) {
      buttons[i].draw(48+(cnt), 118);
      ttt = this.synthCanvas.text(48+(cnt), 80, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 82;
      if((i+1)%3 == 0){
        cnt+=10;
      }
    };

    //second row
    cnt = 0;
    for (var i = 9; i < 17; i++) {
      buttons[i].draw(50+(cnt), 221);
      ttt = this.synthCanvas.text(50+(cnt), 129, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 83;
      if((i)%2 == 0){
        cnt+=18;
        if(i%4==0){
          cnt+=25;
        }
      }
    };

    //third row
    cnt = 0;
    for (var i = 17; i < 25; i++) {
      buttons[i].draw(50+(cnt), 308);
      ttt = this.synthCanvas.text(50+(cnt), 174, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 83;
      if((i)%2 == 0){
        cnt+=18;
        if(i%4==0){
          cnt+=23;
        }
      }
    };

    //4th row 221
    cnt = 0;
    for (var i = 25; i < 32; i++) {
      buttons[i].draw(78+(cnt), 420);
      ttt = this.synthCanvas.text(78+(cnt), 230, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 93;
      if(i == 27 || i == 28){
        cnt+=27;
      }
    };

    //4th row
    cnt = 0;
    for (var i = 32; i < 38; i++) {
      buttons[i].draw(143+(cnt), 535);
      ttt = this.synthCanvas.text(143+(cnt), 286, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 80;
      if((i+1)%2 == 0){
        cnt+=40;
      }
    };

    //5th row
    cnt = 0;
    for (var i = 38; i < 44; i++) {
      buttons[i].draw(143+(cnt), 621);
      ttt = this.synthCanvas.text(143+(cnt), 330, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 80;
      if((i+1)%2 == 0){
        cnt+=40;
      }
    };

    //6th row
    cnt = 0;
    for (var i = 44; i < 48; i++) {
      buttons[i].draw(267+(cnt), 724);
      ttt = this.synthCanvas.text(267+(cnt), 381, butNames[i]);
      ttt.attr({'font-family':'synth','font-size':'18','fill':"#B9E9E1"});
      cnt += 78;
    };
    
    //when touch starts
    Hammer(cs).on("touch", function (event) {
        console.log("start: "+event.gesture.touches.length);

        if(event.gesture.touches.length == 1){

          for (var i = buttons.length - 1; i >= 0; i--) {
            if(buttons[i].getTouch(event.gesture.touches[0].pageX,event.gesture.touches[0].pageY,20)){
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