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

require_once('sitefiltereddisplay.inc');
require_once('filterdisplay.inc');

class searchDisplay extends siteFilteredDisplay
{
   function searchDisplay ($obj_id)
   {
      $this->siteFilteredDisplay($obj_id);
      $this->style_prefix = 'storysummary';
   }

   function processRequest()
   {
      parent::processRequest();

      SharedMemoryUpdate("search");

      $this->integrateFilters();
      $this->display();
   }

   function display()
   {
      $this->writeSearchHeader();
      $this->writeFiltersSection();
      $this->writeDatesSection();
      $this->writeTextFieldsSection();
      $this->writeSubmitSection();
      $this->writeSearchFooter();
   }

   function writeSearchFooter()
   {
      $this->writeDisplayFooter();
   }

   function writeSearchHeader()
   {
      global $graphics_store, $OSCAILT_SCRIPT;
      $this->writeDisplayHeader();

      $search_page_title = "Advanced Newswire Search";
      $search_page_description = "You may select one, some, or all of the search fields below, and then hit the search button at the bottom to perform the search.";

      if ($this->getDisplayOption("search_page_title") != "") 
           $search_page_title = $this->getDisplayOption("search_page_title");

      if ($this->getDisplayOption("search_page_description") != "") 
           $search_page_description = $this->getDisplayOption("search_page_description");


      if(isset($_REQUEST["searchmode"]) && $_REQUEST["searchmode"]=="gallery")
      {
         $lnk = $this->getExtendedLink(array("searchmode=newswire"));
         $srch_id = $this->site_settings['galleryobject'];
         ?>
         <p align="center">
         <img src="<?=$graphics_store?>search.gif" alt='search'> <BIG><b>Advanced Gallery Search</B></BIG> <img src="<?=$graphics_store?>search2.gif" alt='search'>
         <small>
         <br><a href="<?=$lnk?>">newswire search</a> &nbsp;|&nbsp; gallery search
         </p>
         <!--- You may select one, some, or all of the search fields below, and then hit the search button at the bottom to perform the search.-->
         <p><?=$search_page_description?>

         <FORM name="searchform" action="<?=$OSCAILT_SCRIPT?>" method="get">
         <input type='hidden' name='obj_id' value='<?=$srch_id?>'>
         <input type='hidden' name='search_attachments' value='on'>
         <?
      }
      else
      {

         $lnk = $this->getExtendedLink(array("searchmode=gallery"));
         $srch_id = $this->site_settings['newswireobject'];
         ?>
         <p align="center">
         <img src="<?=$graphics_store?>search.gif" alt='search'><BIG><b><?=$search_page_title?></B></BIG> <img src="<?=$graphics_store?>search2.gif" alt='search'>
         <small>
         <br>newswire search &nbsp;|&nbsp; <a href="<?=$lnk?>">gallery search</a></small>
         </p>
         <!--- You may select one, some, or all of the search fields below, and then hit the search button at the bottom to perform the search.-->
         <p><?=$search_page_description?>

         <FORM name="searchform" action="<?=$OSCAILT_SCRIPT?>" method="get">
         <input type='hidden' name='obj_id' value='<?=$srch_id?>'>
         <?
      }
   }


   function writeFiltersSection()
   {
      global $oscailt_page_setting;
      $filterobj = new filterDisplay($this->obj_id);
      $filter_options =
      array(
         "includelanguages" => "true",
         "includetypes" => "true",
         "includemediatypes" => "true",
         "includespecials" => "true",
         "onchange" => "false"
      );
      $oscailt_page_setting['page_filters'] =& $oscailt_page_setting['site_filters'];
      $filterobj->setDisplayOptions($filter_options);
      $selects = array();
      $selects = $filterobj->getPageFilters($selects, "");
      echo "<hr><p><b>Categories</b></P> ";
      echo implode("&nbsp;", $selects);
   }

   function writeDatesSection()
   {
      $search_dates_title = "Restrict Date Range";
      $search_beforeafter = "Before / After";
      $search_dates_description = "Please enter either both or one or none of the before and after date ranges.\n
      Dates may be entered in a variety of formats including the following:";


      if ($this->getDisplayOption("search_dates_label") != "") 
           $search_dates_title = $this->getDisplayOption("search_dates_label");

      if ($this->getDisplayOption("search_beforeafter") != "") 
           $search_beforeafter = $this->getDisplayOption("search_beforeafter");

      if ($this->getDisplayOption("search_dates_description") != "") 
           $search_dates_description = $this->getDisplayOption("search_dates_description");


   ?>
      <HR>
      <!--  Restrict Date Range -->
      <!-- Before / After  -->
      <!-- Please enter either both or one or none of the before and after date ranges.
      Dates may be entered in a variety of formats including the following: -->

      <b><?=$search_dates_title?></b>
      <p>
      <INPUT type=hidden name=convert_dates value="true">
      <B><?=$search_beforeafter?></b>
      <INPUT maxLength=100 name=time_posted_upper_limit value="">
      <INPUT maxLength=100 name=time_posted_lower_limit value="">
      <p>
      <?=$search_dates_description?>
      <ul>
      <li>24 September 2003 14:20
      <li>24 Sept 03 12:07pm
      <li>24 Sep 03
      <li>Sep 24, 2003
      <li>sep 03
      <li>last wednesday
      <li>2 weeks ago
      </ul>
   <?
   }

   function writeTextFieldsSection()
   {

       // These should replace text below in the HTML
       $search_all_fields_label = "Search All Text Fields";
       $search_all_fields_description = "Peforms a case insensitive search of all text.";

       $search_specific_fields_label = "Search Specific Text Fields";
       $search_specific_fields_description = "By default peforms a case insensitive search of specific text fields. Enabling the exact checkbox means that fields must match the search value exactly in that they must be same case, and partial matches are ignored";

       if ($this->getDisplayOption("search_all_fields_label") != "") 
           $search_all_fields_label = $this->getDisplayOption("search_all_fields_label");

       if ($this->getDisplayOption("search_specific_fields_label") != "") 
           $search_specific_fields_label = $this->getDisplayOption("search_specific_fields_label");

       if ($this->getDisplayOption("search_all_fields_description") != "") 
           $search_all_fields_description = $this->getDisplayOption("search_all_fields_description");

       if ($this->getDisplayOption("search_specific_fields_description") != "") 
           $search_specific_fields_description = $this->getDisplayOption("search_specific_fields_description");

      ?>
      <HR>
      <b><?=$search_all_fields_label?></b>
      <p>
      <?=$search_all_fields_description?></p>
      <!--Peforms a case insensitive search of all text.-->
      <p>
      <INPUT maxLength=100 size=50 name=search_text value="">
      <HR>
      <b><?=$search_specific_fields_label?></b>
      <p><?=$search_specific_fields_description?></p>
      <!--Search_specific_fields_description = "By default peforms a case insensitive search of specific text fields. Enabling the exact checkbox means that fields must match the search value exactly in that they must be same case, and partial matches are ignored -->

      <b>Conditional Operator</b>
      <select name=text_where_mode>
         <option value="OR">OR</option>
         <option value="AND">AND</option>
      </select>
      <?

       $author_name_text  = "Author Name";
       $author_org_text   = "Author Organisation";
       $author_addr_text  = "Author Address";
       $author_ph_text    = "Author Phone";
       $author_email_text = "Author Email";
       $story_title_text  = "Story Title";
       $story_subtitle_text="Story Subtitle";
       $story_summary_text= "Story Summary";
       $story_content_text= "Story Content";

       if ($this->getDisplayOption("author_name_label") != "") 
           $author_name_text = $this->getDisplayOption("author_name_label");

       if ($this->getDisplayOption("author_org_label") != "") 
           $author_org_text = $this->getDisplayOption("author_org_label");

       if ($this->getDisplayOption("author_addr_label") != "") 
           $author_addr_text = $this->getDisplayOption("author_addr_label");

       if ($this->getDisplayOption("author_ph_label") != "") 
           $author_ph_text = $this->getDisplayOption("author_ph_label");

       if ($this->getDisplayOption("author_email_label") != "") 
           $author_email_text = $this->getDisplayOption("author_name_label");

       if ($this->getDisplayOption("story_title_label") != "") 
           $story_title_text = $this->getDisplayOption("story_title_label");

       if ($this->getDisplayOption("story_subtitle_label") != "") 
           $story_subtitle_text = $this->getDisplayOption("story_subtitle_label");

       if ($this->getDisplayOption("story_summary_label") != "") 
           $story_summary_text = $this->getDisplayOption("story_summary_label");

       if ($this->getDisplayOption("story_content_label") != "") 
           $story_content_text = $this->getDisplayOption("story_content_label");

       $exact_text= "Exact";

       if ($this->getDisplayOption("exact_label") != "") 
           $exact_text = $this->getDisplayOption("exact_label");
      ?>
      <table>
      <TR>
         <td><small><b><?=$author_name_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=author_name value=""></small></td>
         <td align=left><small><INPUT type=checkbox name=author_name_exact><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$author_org_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name="author_organisation" value=""></small></td>
         <td align=left><small><INPUT type="checkbox" name="author_organisation_exact"><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$author_addr_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=author_address value=""></small></td>
         <td align=left><small><INPUT type=checkbox name=author_address_exact><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$author_ph_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=author_phone value=""></small></td>
         <td align=left><small><INPUT type=checkbox name=author_phone_exact><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$author_email_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=author_email value=""></small></td>
         <td align=left><small><INPUT type=checkbox name=author_email_exact><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$story_title_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=story_title value=""></small></td>
         <td align=left><small><INPUT type=checkbox name=story_title_exact><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$story_subtitle_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=story_subtitle value=""></small></td>
         <td align=left><small><INPUT type=checkbox name=story_subtitle_exact><?=$exact_text?></small></td>
      </TR>
      <TR>
         <td><small><b><?=$story_summary_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=story_summary value=""></small></td>
         <td><small>&nbsp;</small></td>
      </TR>
      <TR>
         <td><small><b><?=$story_content_text?></b></small></td>
         <td><small><INPUT maxLength=100 size=50 name=story_content value=""></small></td>
         <td><small>&nbsp;</small></td>
      </TR>
      </table>
      <?
   }

   function writeSubmitSection()
   {
      global $editor_session;

      $search_comments_text = "search comments also";
      if ($this->getDisplayOption("search_comments_label") != "") 
           $search_comments_text = $this->getDisplayOption("search_comments_label");

      $search_btn_text = "Search";
      if ($this->getDisplayOption("search_button_label") != "") 
           $search_btn_text = $this->getDisplayOption("search_button_label");

      if(isset($_REQUEST['search_comments']) && $_REQUEST['search_comments'] == 'on')
      {
         $search_comments = true;
      }
      else $search_comments = false;

      // Tickbox for generation of links. Available to editors only
      $feature_links_text = "generate links for features ";
      if(isset($_REQUEST['generate_links']) && $_REQUEST['generate_links'] == 'on')
      {
         $generate_links = true;
      }
      else $generate_links = false;

      ?>
      <hr>
      <INPUT type="checkbox" name="search_comments" <? if($search_comments) echo("checked"); ?> ><small> <?=$search_comments_text?></small>
      <?
      if($editor_session->isSessionOpen() )
      {
          ?>
          &nbsp; <INPUT type="checkbox" name="generate_links" <? if($generate_links) echo("checked"); ?> ><small> <?=$feature_links_text?></small>
          <?
      } 
      ?>
      <p>
      <INPUT type=submit value="<?=$search_btn_text?> &gt;&gt;">
      </FORM>
      <?
   }
}
?>
