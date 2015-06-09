<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2124", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Donate To IMEMC");
         addToPageDescription("Support the only 100% independent news source in Palestine - funded solely by your donations!"); ?>

<!-- <Donate to IMEMC obj id 2124> -->
<h1 class='indypagebody'>Donate to keep IMEMC Independent</h1><BLOCKQUOTE class='indypagebody'>The IMEMC is registered as a not-for-profit educational organization in the US. If you are in the United States, just click below to donate directly to our US bank account using your credit or debit card.</BLOCKQUOTE><DIV class='indypagebody'><center>
<a href="https://co.clickandpledge.com/default.aspx?wid=48434" target="_blank"> <img src="http://images.clickandpledge.com/flair/buttons/97x84/CP_EN_OR_A_001.gif" border="0" alt="Secure online donations through ClickandPledge" title ="Online donation system by ClickandPledge">
<br />Click here to donate securely to IMEMC</a><br /><br /></center>

<b><font color="red">For Tax deductible donations, please make your checks payable to "If Americans Knew" and mail them to:</b><br /></font><br />
If Americans Knew<br />
9208 NE Highway 99 Suite 107 - 207<br />
Vancouver WA 98665, USA<br />
(and mention in your memo that the donation is for IMEMC)<br />
Phone: 202-631-4060<br />
Fax: 815-301-8048<br /><br /><br />

<b>For direct donations to PCR in US Dollars, please send your funds to:</b><br /><br />
Pay to: The Bank of New York <br />
Swift Code: IRVTUS3N <br />
New York â?? USA<br />
F/O: Palestine Investment Bank<br />
Swift Code: PINVPS22<br />
A/C No: 890-0719-591<br />
Ramallah â?? Palestine<br />
Ben Name: The Palestinian Centre for Rapprochement between People<br />
Ben A/C No: 78244<br /><br />

<b>For direct donations to PCR's bank account in EURO, please send your funds to:</b>
<br /><br />

Pay to: The Bank of New York Mellon<br />
Frankfurt Branch<br />
Swift Code: IRVTDEFXXXX<br />
Germany<br />
F/O: Palestine Investment Bank <br />
Swift Code: PINVPS22<br />
A/C No: 667 048 9710<br />
Ramallah â?? Palestine<br />
Ben Name: The Palestinian Centre for Rapprochement between People<br />
Ben A/C No: 78244<br /><br />

<center>If you have questions, please contact us at the following e-mail <a href="mailto:donation@imemc.org">donation@imemc.org</a></center></DIV>
<!-- </Donate to IMEMC obj id 2124> -->

<?php getPageLanguageOptions("en", Array("0" => "en", "1" => "sp"), "2124"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2124&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [Donate to IMEMC] Configuration" alt="Edit Static Document [Donate to IMEMC] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          