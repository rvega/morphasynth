#ifndef SLIDERPOT
#define SLIDERPOT

#include "ofxUIWidgetWithLabel.h"
#include <math.h>

class SliderPot : public ofxUIWidgetWithLabel
{
public:
    SliderPot() {//std::cout << "constructor\n";
}
//----------------------------------------------------------------------------------------    
    SliderPot(float w, float h, float _min, float _max, float _value, float n, string _name){

/*
//para la proxima TODO: preguntarle a Rafael
ofxUiSliderPot :: ofxUiSliderPot (float w, float h, float _min, float _max, float _value, ,float n, string _name) : ofxUISLider (float w, float h, float _min, float _max, float _value, ,float n, string _name)
{ 
// Cuerpo del constructor de la clase Derivada
}*/	 

	  nPow = n;

	//a y b determina el intervalo de valores que se le va a pasar al sinte
	  a= _min;
	  b= _max;	
	  rect = new ofxUIRectangle(0,0,w,h); 
//        init(w, h, _min, _max, _value, _name); 
 	  init(w, h, 0.0, 1.0, _value, _name); 

//	  initPot(w, h, _min, _max, _value, _name);
}
  
//-----------------------------------------------------------------------------------------

float map(float value, float low1, float high1, float low2, float high2) {
             return (value / (high1 - low1)) * (high2 - low2) + low2;
              }    
    
//-----------------------------------------------------------------------------------------
    virtual void init(float w, float h, float _min, float _max, float _value, string _name)
    {
        name = _name; 				
		if(w > h)
		{
			kind = OFX_UI_WIDGET_SLIDER_H;  			
		}
		else 
		{
			kind = OFX_UI_WIDGET_SLIDER_V;  			
		}
        
		paddedRect = new ofxUIRectangle(-padding, -padding, w+padding*2.0, h+padding);
		paddedRect->setParent(rect);     
        
        draw_fill = true; 
        
        value = _value;                                               //the widget's value
		max = _max; 
		min = _min; 
        labelPrecision = 6;
        
		if(value > max)
		{
			value = max; 
		}
		if(value < min)
		{
			value = min; 
		}

		
		
		
		//value = ofMap(value, min, max, 0.0, 1.0, true);
		
		
		
		
        
		if(kind == OFX_UI_WIDGET_SLIDER_H)
		{
			label = new ofxUILabel(0,h+padding,(name+" LABEL"), (name + ": " + ofToString(max,labelPrecision)), OFX_UI_FONT_SMALL); //aqui deberia ser max=1
		}
		else 
		{
			label = new ofxUILabel(0,h+padding,(name+" LABEL"), name, OFX_UI_FONT_SMALL); 	
		}
        
		label->setParent(label); 
		label->setRectParent(rect); 
        increment = 1.0;         
    }

    virtual void setDrawPadding(bool _draw_padded_rect)
	{
		draw_padded_rect = _draw_padded_rect; 
        label->setDrawPadding(false);
	}
    
    virtual void setDrawPaddingOutline(bool _draw_padded_rect_outline)
	{
		draw_padded_rect_outline = _draw_padded_rect_outline; 
        label->setDrawPaddingOutline(false);
	}  
    
    virtual void drawBack() 
    {
        if(draw_back)
        {
            ofFill(); 
            ofSetColor(color_back); 
            rect->draw(); 
        }
    }
    
    virtual void drawOutline() 
    {
        if(draw_outline)
        {
            ofNoFill();
            ofSetColor(color_outline); 
            rect->draw(); 
        }
    }
    
    virtual void drawOutlineHighlight() 
    {
        if(draw_outline_highlight)
        {
            ofNoFill();
            ofSetColor(color_outline_highlight); 
            rect->draw();          
        }
    }    
    
    virtual void drawFill() 
    {
        if(draw_fill)
        {			
            ofFill(); 
            ofSetColor(color_fill); 
			if(kind == OFX_UI_WIDGET_SLIDER_H)
				
			{	
				ofRect(rect->getX(), rect->getY(), rect->getWidth()*value, rect->getHeight()); 
			}
			else 
			{
				ofRect(rect->getX(), rect->getY()+rect->getHeight(), rect->getWidth(), -rect->getHeight()*value); 
			}
        }
    }
    
    virtual void drawFillHighlight() 
    {
        if(draw_fill_highlight)
        {
            ofFill(); 
            ofSetColor(color_fill_highlight); 
			if(kind == OFX_UI_WIDGET_SLIDER_H)
				
			{	
				ofRect(rect->getX(), rect->getY(), rect->getWidth()*value, rect->getHeight()); 
			}
			else 
			{
				ofRect(rect->getX(), rect->getY()+rect->getHeight(), rect->getWidth(), -rect->getHeight()*value); 
			}	
			if(kind == OFX_UI_WIDGET_SLIDER_V)
			{
				label->drawString(rect->getX()+rect->getWidth()+padding, label->getRect()->getHeight()/2.0+rect->getY()+rect->getHeight()-rect->getHeight()*value, ofToString(getScaledValue(),labelPrecision)); 
			}
        }        
    }
        
    void mouseMoved(int x, int y ) 
    {
        if(rect->inside(x, y))
        {
            state = OFX_UI_STATE_OVER;         
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;        
        }
        stateChange();         
    }
    
    void mouseDragged(int x, int y, int button) 
    {
        if(hit)
        {
            state = OFX_UI_STATE_DOWN;     
			input(x, y); 
			triggerEvent(this); 			
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;        
        }
        stateChange();     
    }
    
    void mousePressed(int x, int y, int button) 
    {
        if(rect->inside(x, y))
        {
            hit = true; 
            state = OFX_UI_STATE_DOWN;     
			input(x, y); 
			triggerEvent(this); 
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;        
        }
        stateChange();         
    }
    
    void mouseReleased(int x, int y, int button) 
    {
        if(hit)
        {
#ifdef TARGET_OPENGLES
            state = OFX_UI_STATE_NORMAL;        
#else            
            state = OFX_UI_STATE_OVER; 
#endif            
			input(x, y); 
			triggerEvent(this); 			
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;         
        }
        stateChange();    
        hit = false; 
    }
	
/**  
  void keyPressed(int key) 
    {		
		if(state == OFX_UI_STATE_OVER)
		{
			switch (key) 
			{
				case OF_KEY_RIGHT:
					setValue(getScaledValue()+increment); 
					triggerEvent(this); 
					break;
                    
				case OF_KEY_UP:
					setValue(getScaledValue()+increment); 
					triggerEvent(this); 
					break;
					
				case OF_KEY_LEFT:
					setValue(getScaledValue()-increment); 					
					triggerEvent(this); 
					break;
                    
				case OF_KEY_DOWN:
					setValue(getScaledValue()-increment); 					
					triggerEvent(this); 
					break;					
                    
				default:
					break;
			}
		}
    }
    
    void keyReleased(int key) 
    {
		
    }
	*/

    void windowResized(int w, int h) 
    {
		
    }
    
    float getIncrement()
    {
        return increment; 
    }
    
    void setIncrement(float _increment)
	{
		increment = _increment; 
	}
    
	virtual void input(float x, float y)
	{
		if(kind == OFX_UI_WIDGET_SLIDER_H)
		{	
			value = rect->percentInside(x, y).x; 
		}
		else 
		{
			value = 1.0-rect->percentInside(x, y).y; 
		}	
        
        if(value > 1.0)
        {
            value = 1.0;
        }
        else if(value < 0.0)
        {
            value = 0.0;
        }        

		updateLabel(); 
	}

//getScaledValue a getValue pow
	virtual void updateLabel()
	{
		if(kind == OFX_UI_WIDGET_SLIDER_H)
		{

			valuePow = pow(value, nPow);
			//value = ofMap(valuePow, min, max, 0.0, 1.0, true);
			label->setLabel(name + ": " + ofToString(valuePow,labelPrecision)); 		
		}		
	}
	
    void stateChange()
    {                
        switch (state) {
            case OFX_UI_STATE_NORMAL:
            {            
                draw_fill_highlight = false;             
                draw_outline_highlight = false;             
				label->unfocus(); 				
            }
                break;
            case OFX_UI_STATE_OVER:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = true;    
				label->unfocus(); 
            }
                break;
            case OFX_UI_STATE_DOWN:
            {
                draw_fill_highlight = true;            
                draw_outline_highlight = true;             
				label->focus(); 				
            }
                break;
            case OFX_UI_STATE_SUSTAINED:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = false;
				label->unfocus(); 				
            }
                break;            
                
            default:
                break;
        }        
    }
	
	void setValue(float _value)
	{	
		
		value = ofMap(value, min, max, 0.0, 1.0, true);
				
		updateLabel(); 		
	}
		
	float getValue()
	{
			
		return value; 
	}
	
	float getPercentValue()
	{
		return value; 
	}
	
	float getMapValue()
	{
		return ofMap(valuePow, 0.0, 1.0, a, b, true); 
	}

	float getScaledValue()
	{
		return ofMap(value, 0.0, 1.0, min, max, true); 
	}
    
	ofxUILabel *getLabel()
	{
		return label; 
	}
    
    void setLabelVisible(bool _labelVisible)
    {
        label->setVisible(_labelVisible);
    }
    
    void setVisible(bool _visible)
    {
        visible = _visible; 
        label->setVisible(visible); 
    }
	
	virtual void setParent(ofxUIWidget *_parent)
	{
		parent = _parent; 
		paddedRect->height += label->getPaddingRect()->height; 
        if(label->getPaddingRect()->width > paddedRect->width)
        {
            paddedRect->width = label->getPaddingRect()->width;				
        }
        updateLabel(); 
	}
	
    void setLabelPrecision(int _precision) {
        labelPrecision = _precision;
        updateLabel();
    }
    
    void setMax(float _max)
    {
        setMaxAndMin(_max, min); 
    }

    float getMax()
    {
        return max; 
    }
    void setMin(float _min)
    {
        setMaxAndMin(max, _min); 
    }
    
    float getMin()
    {
        return min; 
    }
    
    ofVec2f getMaxAndMind()
    {
        return ofVec2f(max, min); 
    }
    
    void setMaxAndMin(float _max, float _min)
    {
        max = _max; 
        min = _min; 
		
		//value = ofMap(valuePow, 0, 1.0, min, max, true);         
		value = ofMap(valuePow, min, max, 0.0, 1.0, true); 
        updateLabel(); 
    }

    bool isDraggable()
    {
        return true; 
    }
    
protected:    //inherited: ofxUIRectangle *rect; ofxUIWidget *parent; 
	
	// value nos va a permitir guardar el valor elevado a la n
	float value, increment; 
	float nPow;
	// generamos un nuevo valuePow para suplir las funciones de la clase en relacion a 		al interfaz
	float valuePow;
	float max, min;
	float a, b;  
        int labelPrecision;
}; 

#endif
