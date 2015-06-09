<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

class GenerateBarGrp
{
   var $width = 500;
   var $height = 110;
   var $title_x_offset = 0;
   var $grph_title;
   var $currency;
   var $fund_text;
   var $target_funds;
   var $link_page;
   var $base_image_filename;
   var $always_generate = true;
   var $show_targ_bar = true;
   var $highlight_target = false;
   var $background_color;
   var $targetfund_color;
   var $fundsraised_color;

   function GenerateBarGrp()
   {
      $this->grph_title = "Indymedia Ireland WebServer Hosting Fundraising Appeal for 2008/2009";
      $this->link_page  = "donate.php";
      $this->currency   = " Euros";
      $this->base_image_filename = "/fundsbargraph_";

      // Cyan in fact...
      $this->background_color = array(200, 215, 255);
      $this->targetfund_color = array(255, 144, 144);
      $this->fundsraised_color= array(20 , 160, 20 );
   }
   function generateBar($target_funds=0, $funds_raised=0,$compact_grph = false)
   {
      $this->target_funds = $target_funds;
      // If target_funds is passed as zero then assume it was set earlier and hence fund_text too.
      if ($target_funds > 0 && $this->fund_text == NULL)
          $this->fund_text = "Funding target is ".$target_funds . $this->currency;

      if ($compact_grph == true) {
          $this->height = 80;
          $title_level = 8;
          $bar_height = 15;
          $level_1 = 15;
          $level_2 = 45;
      } else {
          $bar_height = 20;
          $title_level = 12;
          $level_1 = 30;
          $level_2 = 70;
      }

      // The full graph width will be total -10 for each end.
      if ($target_funds == 0) {
         $target_funds = 380;
         $target_width = 380;
         $funds_raised = 200;
         $funds_width = 200;
      } else {
         // Normalize the range
         $target_width = $this->width - (2*10);
	 // In normalization account for the missing 10 pixels at the start too.
         if ( $funds_raised <= $target_funds ) {
             $funds_width  = round($funds_raised * ($target_width/$target_funds) );
         } else {
             $funds_width  = $this->width -(2*10);
             $target_width = round($target_funds * ($funds_width/$funds_raised) );
         }
      }

      $im=imagecreate($this->width, $this->height) OR die ("Cannot generate fund raising bargraph image");
      if ($im === FALSE OR $im == null ) return null;

      //
      $black=imagecolorallocate($im, 0, 0, 0);
      $grey =imagecolorallocate($im, 220, 220, 220);
      //$green=imagecolorallocate($im, 20 , 160, 20 );
      //$green=imagecolorallocate($im, 187, 187, 255);
      //$red  =imagecolorallocate($im, 255, 174, 174);
      //$red  =imagecolorallocate($im, 255, 144, 144);

      // Cyan in fact...
      $bg_clr=imagecolorallocate($im, $this->background_color[0],$this->background_color[1],$this->background_color[2]);
      $green=imagecolorallocate($im, $this->fundsraised_color[0],$this->fundsraised_color[1],$this->fundsraised_color[2]);
      $targfund_clr =imagecolorallocate($im, $this->targetfund_color[0],$this->targetfund_color[1],$this->targetfund_color[2]);

      // Draw border, by a full fill and then a smaller inner rectangle with difference being the border.
      $this->initImageBorder($im, $black, $bg_clr);

      $t_x = $this->getCenterCoords(3, strlen($this->grph_title));
      $t_x = $t_x - $this->title_x_offset;
      imagestring($im, 3, $t_x, $title_level, $this->grph_title, $black);

      // Draw bargraph for the target funds to be raised
      if ($this->highlight_target == true) {
          imagefilledrectangle($im, 10, 10+$level_1, 10+$target_width, 10+$level_1+$bar_height, $targfund_clr);
      }
      imagestring($im, 3, $this->width/4, 12+$level_1, $this->fund_text, $black);

      if ($this->show_targ_bar == true) {
          // Draw a full gray box first.
          imagefilledrectangle($im, 10, 10+$level_2, 10+$target_width, 10+$level_2+$bar_height, $targfund_clr);
      }
      imagefilledrectangle($im, 10, 10+$level_2, 10+$funds_width, 10+$level_2+$bar_height, $green);

      $t_funds_raised = "Funds raised so far ".$funds_raised . $this->currency;
      imagestring($im, 3, $this->width/4, 12+$level_2, $t_funds_raised, $black);
      if ( $funds_raised > $target_funds) {
          imagefilledrectangle($im, 8+$target_width, 10+$level_2, 10+$target_width, 10+$level_2+$bar_height, $targfund_clr);
      }

      return $im;
   }

   function initImageBorder($img_res, $border_color, $bg_color)
   {
      // Draw border, by a full fill and then a smaller inner rectangle with difference being the border.
      imagefilledrectangle($img_res, 0, 0, $this->width, $this->height, $border_color);
      imagefilledrectangle($img_res, 2, 2, $this->width-2, $this->height-2, $bg_color);
   }

   // Just returns x co-ord centered.
   function getCenterCoords($font_used, $str_length)
   {
       $t_center_x = ceil( ($this->width - (ImageFontWidth($font_used) * $str_length) ) / 2 );
       return $t_center_x; 
   }

 
   function setTitleXoffset($new_value)
   {
       $this->title_x_offset = $new_value;
   }

   function setWidthHeight($new_width, $new_height)
   {
       $this->width = $new_width;
       $this->height= $new_height;
   }

   function setBackgroundColor($t_red=128, $t_green=128, $t_blue=128)
   {
       if ($t_red   < 0 || $t_red > 255 ) $t_red = 128;
       if ($t_green < 0 || $t_green > 255 ) $t_green = 128;
       if ($t_blue  < 0 || $t_blue > 255 ) $t_blue = 128;

       $this->background_color = array($t_red, $t_green, $t_blue);
   }
   function setTargetFundColor($t_red=128, $t_green=128, $t_blue=128)
   {
       if ($t_red   < 0 || $t_red > 255 ) $t_red = 128;
       if ($t_green < 0 || $t_green > 255 ) $t_green = 128;
       if ($t_blue  < 0 || $t_blue > 255 ) $t_blue = 128;

       $this->targetfund_color = array($t_red, $t_green, $t_blue);
   }
   function setFundRaisedColor($t_red=128, $t_green=128, $t_blue=128)
   {
       if ($t_red   < 0 || $t_red > 255 ) $t_red = 128;
       if ($t_green < 0 || $t_green > 255 ) $t_green = 128;
       if ($t_blue  < 0 || $t_blue > 255 ) $t_blue = 128;

       $this->fundsraised_color = array($t_red, $t_green, $t_blue);
   }


   function setUpdateMode($always_update = false) { $this->always_generate = $always_update; }
   function setCurrencyText($currency_text) { $this->currency = $currency_text; }
   function setTitle($new_title) { $this->grph_title = $new_title; }
   function setLinkPage($link_page) { $this->link_page = $link_page; }
   function setGraphBasename($grph_base) { $this->base_image_filename = $grph_base; }

   function setFundText($new_fund_text,$target_funds=0)
   {
       // Assume if they pass zero for target funds that we use the existing value set at the start.
       if ($target_funds == 0 ) 
           $this->fund_text = $new_fund_text .$this->target_funds . $this->currency;
       else
           $this->fund_text = $new_fund_text .$target_funds . $this->currency;
   }


   function getGraphFilename($funds_raised, $is_compact)
   {
       global $system_config;

       if ($is_compact == true ) {
           $graph_filename = $system_config->image_cache . "/".$this->base_image_filename.$funds_raised."_1.jpg";
       } else {
           $graph_filename = $system_config->image_cache . "/".$this->base_image_filename.$funds_raised."_0.jpg";
       }
       return $graph_filename;
   }

   function writeBarGraphHTML($graph_filename, $create_link)
   {
       global $system_config;

       if ($create_link == true ) {
           ?><p align=center><a href="http://<?=$system_config->site_url?>/<?=$this->link_page?>"><img src="<?=$graph_filename?>" /></a></p> <?
       } else {
           ?><p align=center><img src="<?=$graph_filename?>" /></p> <?
       }
   }


   function displayBarGraph($target_funds=1000, $funds_raised=0, $compact_grph=false,$create_link=false)
   {
       $ret_code = true;

       $graph_filename = $this->getGraphFilename($funds_raised, $compact_grph);

       if ($this->always_generate == true || !file_exists($graph_filename) ) {
           $bar_graph_im = $this->generateBar($target_funds, $funds_raised, $compact_grph);
	   if ($bar_graph_im == null) 
	       $ret_code = false;
	   else 
	       imagejpeg($bar_graph_im, $graph_filename,100);
       }

       $this->writeBarGraphHTML($graph_filename, $create_link);

       return $ret_code;
   }
}

   // $barGrp = new GenerateBarGrp();
   // $barGrp->displayBarGraph(580, 500); 

class GenerateLineGrp extends GenerateBarGrp
{
   var $grey_bar = true;
   var $height_range = 80;

   function GenerateLineGrp()
   {
      $this->grph_title = "Indymedia Ireland WebServer Memory Usage Sampling";
      $this->link_page  = "donate.php";
      $this->base_image_filename = "/linegraph_";

      // Cyan in fact...
      $this->background_color = array(200, 215, 255);
      $this->targetfund_color = array(255, 144, 144);
      $this->fundsraised_color= array(20 , 160, 20 );
   }
   function setHeightRange($new_range)
   {
       $this->height_range = $new_range;
   }

   // Input data format is a one dimension list.
   function generateLine($data_list, $compact_grph = false, $plot_index=false)
   {
      // Default set in parent 
      if ($compact_grph == true) {
          $this->height = 80;
          $title_level = 8;
          $this->height_range = 70;
          $level_1 = 15;
          $level_2 = 45;
      } else {
          $title_level = 10;
          $level_1 = 30;
          $level_2 = 70;
      }

      $im=imagecreate($this->width, $this->height) OR die ("Cannot generate line graph image");
      if ($im === FALSE OR $im == null ) return null;
      //
      $black=imagecolorallocate($im, 0, 0, 0);
      //$white=imagecolorallocate($im, 255, 255, 255);
      // Cyan in fact...
      $white=imagecolorallocate($im, 200, 200, 200);
      $grey =imagecolorallocate($im, 220, 220, 220);

      $green=imagecolorallocate($im, 20 , 140, 20 );
      //$green=imagecolorallocate($im, 187, 187, 255);
      //$red  =imagecolorallocate($im, 255, 174, 174);
      $red  =imagecolorallocate($im, 255, 144, 144);

      // Draw border - must be done at the start.
      $this->initImageBorder($im, $black, $white);

      // write main covering text: image, font, x, y, text, color
      $t_x = $this->getCenterCoords(3, strlen($this->grph_title));
      imagestring($im, 3, $t_x, $title_level, $this->grph_title, $black);

      $data_offset = 28;
      $spacing = round( ($this->width-$data_offset) / count($data_list));
      // Get Max.
      $max_value = 0;
      foreach ($data_list as $datum ) {
	  if ($datum > $max_value ) $max_value = $datum;
      }
      // Draw each line for each element
      $x1 = $data_offset;
      $level_3 = $this->height -5;
      $totalCount = count($data_list);
      // Draw grid.
      $gridCount = round($this->height_range/20);
      $grid_pos = $level_3;
      $label = 0;
      for($iPt=0; $iPt < $gridCount;$iPt++) {
          imageline($im, 30, $grid_pos, ($this->width-20), $grid_pos, $grey);
          imagestring($im, 1, 4, $grid_pos-5, $label."k", $black);
          $grid_pos = $grid_pos - 20;
          $label = $label + round($max_value/$gridCount);
      }

      //imageline($im, 10, 50, ($width-20), 50, $grey);
      //imageline($im, 10, 70, ($width-20), 70, $grey);
      //imageline($im, 10, 90, ($width-20), 90, $grey);
      //imageline($im, 10, 110, ($width-20), 110, $grey);
      for($iPt=1; $iPt < $totalCount;$iPt++) {
          // Normalize data.
          $x2 = $x1 + $spacing;
	  $y1 = round(($data_list[$iPt-1] / $max_value ) * $this->height_range);
	  $y2 = round(($data_list[$iPt] / $max_value ) * $this->height_range);
          imageline($im, $x1, $level_3-$y1, $x2, $level_3-$y2, $red);
          //imagesetpixel($im, $x1, 10+$y1, $green);
	  //echo("i ".$iPt." data ".$data_list[$iPt-1]." x1 ".$x1." y1 ". ($level_3-$y1) . " y2 ".($level_3-$y2)."<BR>");
          imagefilledrectangle($im, $x1, $level_3-$y1, $x1+2, $level_3-($y1-2), $green);
          $x1 = $x1 + $spacing;
      }

      return $im;
   }

   function displayLineGraph($data_list, $compact_grph=false,$create_link=false, $plot_index=false)
   {
       if ($data_list == null ) { echo("Input data is null. Cannot generate line graph.<br>"); return; }

       $graph_filename = $this->getGraphFilename(0, $compact_grph, $plot_index);

       if ($this->always_generate == true || !file_exists($graph_filename) ) {
           $bar_graph = $this->generateLine($data_list, $compact_grph);
   	   if ($bar_graph == null) 
	       return false;
	   else 
	       imagejpeg($bar_graph, $graph_filename,100);
       }

       $this->writeBarGraphHTML($graph_filename, $create_link);
       return true;
   }
}

?>
