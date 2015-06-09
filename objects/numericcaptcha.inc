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
      var $pos_mask;

      function NumericCaptcha()
      {
         $this->max_digits = 3;
	 // Each one cannot be more than 7
         $this->pos_mask =  array( 0 => 3, 1 => 2, 2 => 6 );
         $this->jumble_mask = "2384710596";
         //his->jumble_mask = "0123456789";
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
             switch ( $this->number[$icount] )
	     {
                case 0:
                 $this->numberString[$icount] = "zero";
                 break;

                case 1:
                 $this->numberString[$icount] = "one";
                 break;

                case 2:
                 $this->numberString[$icount] = "two";
                 break;

                case 3:
                 $this->numberString[$icount] = "three";
                 break;

                case 4:
                 $this->numberString[$icount] = "four";
                 break;

                case 5:
                 $this->numberString[$icount] = "five";
                 break;

                case 6:
                 $this->numberString[$icount] = "six";
                 break;

                case 7:
                 $this->numberString[$icount] = "seven";
                 break;

                case 8:
                 $this->numberString[$icount] = "eight";
                 break;

                case 9:
                 $this->numberString[$icount] = "nine";
                 break;

                default:
                 $this->numberString[$icount] = "point";
             }
         }
      }

      function getNumericString()
      {
         $this->convertToNumericString();

	 $text_str = "";
	 for ($icount = 0; $icount < $this->max_digits; $icount++)
         {
	     $text_str .= $this->numberString[$icount] . " ";
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
