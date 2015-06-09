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

   class NumericCaptcha
   {
      var $number;
      var $numberString;
      var $max_digits;
      var $coded_number;
      var $jumble_mask;
      var $jumble_disp;
      var $jumble_word;
      var $pos_mask;
      var $lingo;
      var $numberTexts;

      function NumericCaptcha()
      {
         $this->max_digits = 3;
	 // Each one cannot be more than 7
         $this->pos_mask =  array( 0 => 3, 1 => 2, 2 => 6, 3 => 1, 4 => 5, 5 => 4 );
         $this->jumble_mask = "2384710596";
         //his->jumble_mask = "0123456789";
         $this->jumble_disp = true;
         $this->jumble_word = true;

	 global $userLanguage;
         $this->lingo = "en";
         if ($userLanguage != null) {
              $this->lingo = $userLanguage->getLanguageCodePrefix();
	 }

	 if ($this->lingo == "fr") {
	 	$this->numberTexts = Array( 0 => " zéro", 1 => "un", 2 => "deux", 3 => "trois", 4 => "quatre", 
			          5 => "cinq", 6 => "six", 7 => "sept", 8 => "huit", 9 => "neuf");
         } else if ($this->lingo == "es" || $this->lingo == "ca") {
	 	$this->numberTexts = Array( 0 => "cero", 1 => "uno", 2 => "dos", 3 => "tres", 4 => "cuatro", 
			          5 => "cino", 6 => "seis", 7 => "siete", 8 => "ocho", 9 => "nueve");
         } else if ($this->lingo == "it") {
	 	$this->numberTexts = Array( 0 => "zero", 1 => "uno", 2 => "due", 3 => "tre", 4 => "quattro", 
			          5 => "cinque", 6 => "sei", 7 => "sette", 8 => "otto", 9 => "nove");
         } else if ($this->lingo == "pt") {
	 	$this->numberTexts = Array( 0 => "zero", 1 => "um", 2 => "dois", 3 => "três", 4 => "quatro", 
			          5 => "cinco", 6 => "seis", 7 => "sete", 8 => "oito", 9 => "nove");
         } else if ($this->lingo == "de") {
	 	$this->numberTexts = Array( 0 => "null", 1 => "eins", 2 => "zwei", 3 => "drei", 4 => "vier", 
			          5 => "fünf", 6 => "sechs", 7 => "sieben", 8 => "acht", 9 => "neun");
         } else {
	 	$this->numberTexts = Array( 0 => "zero", 1 => "one", 2 => "two", 3 => "three", 4 => "four", 
			          5 => "five", 6 => "six", 7 => "seven", 8 => "eight", 9 => "nine");
         }

      }

      function generateCaptcha()
      {
	 // 1000000,9990000
	 $captcha = substr(rand(1000000,9990000),(7-$this->max_digits),$this->max_digits);;

         $this->number = array();
         $this->coded_number = "";
	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
             $cur_digit = substr($captcha, $icount, 1);
             $this->number[$icount] = $cur_digit;
             $this->coded_number .= substr(rand(1000000,9990000), 0, $this->pos_mask[$icount]);
             $this->coded_number .= substr($this->jumble_mask,$cur_digit,1);
         }
         $this->coded_number .= rand(1000000,9990000);
      }

      function convertToNumericString()
      {
         $this->numberString = array();
	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
             if ( $this->number[$icount] >= 0 && $this->number[$icount] <= 9 ) 
	         $this->numberString[$icount] = $this->numberTexts[$this->number[$icount]];
	     else
                 $this->numberString[$icount] = "point";
         }
      }

      function getNumericString()
      {
         $this->convertToNumericString();

	 $text_str = "";
	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
	     $t_num_str = $this->numberString[$icount];
	     if ($this->jumble_word == true) {
		  $pos_r = rand(1,strlen($this->numberString[$icount]));
	          $t_num_str = substr($t_num_str,0,$pos_r) . "-" . substr($t_num_str,$pos_r);
	     }
	     if ($this->jumble_disp == true) {
		 // $t_max = strlen($this->numberString[$icount]);
		 $t_max = strlen($t_num_str);
	         $pos_r = rand(0,11);
		 if ($pos_r < $t_max && substr($t_num_str,$pos_r,1) != "-") {
		     if ($pos_r == 0) {
		         $t_num_str = "&#" . ord(substr($t_num_str,0,1)) .";". substr($t_num_str,1);
		     } else if ($pos_r == ($t_max-1)) {
	                 $t_num_str = substr($t_num_str,0,($t_max-1)) . "&#" . ord(substr($t_num_str,-1)) .";";
		     } else {
	                 $t_num_str = substr($t_num_str,0,$pos_r)."&#".ord(substr($t_num_str,$pos_r,1)) .";". substr($t_num_str,($pos_r+1));
		     }
		 }
	     }

	     if ($this->jumble_disp == true) {
	         $html_mode = rand(0,3);
		 switch ($html_mode) {
		 	case 0:
			 	$open_tag = "<b>";
			 	$close_tag = "</b>";
			 	break;
		 	case 1:
			 	$open_tag = "<i>";
			 	$close_tag = "</i>";
			 	break;
		 	case 2:
			 	$open_tag = "<u>";
			 	$close_tag = "</u> ";
			 	break;
		 	case 3:
		 	default:
			 	$open_tag = "<font size='+1'>";
			 	$close_tag = "</font>";
			 	break;
		 }
	         $text_str .= $open_tag . $t_num_str . $close_tag . " ";
	         if ($icount < ($this->max_digits-1) ) {
	             $html_mode = rand(0,3);
		     if ($html_mode > 0 ) $text_str .= "&nbsp;";
	         }
	     } else {
	         // $text_str .= $this->numberString[$icount] . " ";
	         $text_str .= $this->numberString[$icount] . " ";
	     }
         }
	 return $text_str;
      }

      function validateNumbers()
      {
         $all_present = true;
	 $digits_str = "";
	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
             switch ($icount)
	     {
                 case 0:
                     if(!isset($_REQUEST['cap_digit_1'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_1'];

                 case 1:
                     if(!isset($_REQUEST['cap_digit_2'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_2'];

                 case 2:
                     if(!isset($_REQUEST['cap_digit_3'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_3'];

                 case 3:
                     if(!isset($_REQUEST['cap_digit_4'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_4'];

                 case 4:
                     if(!isset($_REQUEST['cap_digit_5'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_5'];

                 case 5:
                     if(!isset($_REQUEST['cap_digit_6'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_6'];

                 case 6:
                     if(!isset($_REQUEST['cap_digit_7'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_7'];

                 case 7:
                     if(!isset($_REQUEST['cap_digit_8'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_8'];

                 case 8:
                     if(!isset($_REQUEST['cap_digit_9'])) $all_present = false;
		     else $digits_str .= $_REQUEST['cap_digit_9'];
	     }
         }
	 if ($all_present == false ) return false;

	 return true;
      }

      function validateCaptchaData($user_digits, $form_mask_digits)
      {
         if (strlen($user_digits) > $this->max_digits ) return false;
         if (strlen($user_digits) < $this->max_digits ) return false;

         $decode_number = "";
         $mask_digits = "";
         $pos_total = 0;

 	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
             $pos_total = $pos_total + $this->pos_mask[$icount];
             $mask_digits .= substr($form_mask_digits, $pos_total, 1);
             $pos_total++;
         }
 	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
             $cur_digit = substr($mask_digits, $icount, 1);
             $digit_pos = strpos($this->jumble_mask, $cur_digit);
	     if ($digit_pos === false ) {
                 return false;
	     }
             $decode_number .= $digit_pos;
         }

         if ( strcmp( $user_digits, $decode_number) == 0)
         {
             return true;
         }
         return false;
      }

      function getFormHTML()
      {
	 $form_txt = "<input type=\"text\" name=\"cap_digits1\" value=\"\" size=\"".$this->max_digits."\" maxlength=\"".$this->max_digits."\">\n";

	 $form_txt .= "<input type=\"hidden\" name=\"cap_digits2\" value=\"".$this->coded_number."\">\n";
         return $form_txt;
      }
   }
?>
