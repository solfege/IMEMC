<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2042", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Privacy");
         addToPageDescription("International Middle East Media Center Privacy Policy."); ?>

<!-- <privacy obj id 2042> -->
<h1 class='indypagebody'>International Middle East Media Center Privacy Policy</h1><BLOCKQUOTE class='indypagebody'>The International Middle East Media Center does not normally retain server logs of activity on the website.   In special cases, where persistent patterns of abuse are noticed, we may decide to use the logs to identify the source of the abuse. The information in these logs is not shared with any other body and is not used for any other purposes other than ensuring the secure functioning of the site.</BLOCKQUOTE><DIV class='indypagebody'>The International Middle East Media Center (IMEMC) web site contains links to other websites, whether controlled by Indymedia affiliates or unrelated third parties. IMEMC is not responsible for the privacy policies and practices of any linked website. 
<P> 
Currently, the IMEMC site uses personal information as a basis for identifying the authors of published articles, and in order to allow individual user login to certain site functions (e.g. to administer the newswire). Names (and additional information that a user may choose to post) are publicly available on the IMEMC site. 
<P> 
All email sent to the IMEMC mailing list is stored and publicly accessible at the list archive, including the sender's reported name and e-mail address. 
<P><b>Subscription-based News Emails</b> 
<P> 
Users may subscribe to receive site notifications, newsletters or other information via email. Users may remove themselves from such services at their discretion. Any email sent to an Indymedia list will include directions for how a user may remove him or herself from the list, or a URL where the user may find such directions. 

<P><b>Other Notification</b> 
<P> 
In order to arrange for the production of web content, print publications, and audio and video features, Indymedia Ireland may use personal information to contact users on an individual basis. 

<P><b>Name, Email, Phone Number, Address Display</b> 
<P> 
Users' personal information may be publicly available through published articles. Users have the option to opt out of publicly displaying their real names by typing in a pseudonym; users may choose to not disclose address or telephone contact information at all. Requests to remove personal identifying information from published articles must be sent using the contact form 

<P><b>General</b> 
<P> 
In cases where users voluntarily and publicly disclose personal information in conjunction with content published subject to the IMEMC content usage policy, such personal information may or may not be disclosed as part of a non-commercial assemblage of Indymedia content, whether in print, electronic, audio or video. The producers of any such compilation will make every effort to contact the original author to determine whether credit/disclosure is desired. 
<P> 
At no time, unless a user specifically authorizes such disclosure, will Indymedia Ireland disclose individual user personal information that is not publicly available to unrelated third parties. 
<P> 
The IMEMC email lists, hosted at lists.indymedia.org, are configured such that all text messages sent to the list, along with names and email addresses, are disclosed in the message archive. In addition, a complete list of subscribers may be available to third parties at the lists web site, although users can opt to conceal themselves from the subscriber list. Users concerned with privacy issues are encouraged to sign up for an anonymous email service. 
<P> 
<b>Security</b> 
<P> 
To secure site integrity, the IMEMC employs measures including but not limited to security audits, use of encryption tools and software, and other reasonable security measures and procedures. Internal access to web server access logs and any non-public user information is restricted to site administrators and individuals on a need-to-know basis. 

<P> 
<b>Enforcement</b> 
<P> 
In the event that the IMEMC becomes aware that site security is compromised or non-public user information has been disclosed to unrelated third parties as a result of external activity, including but not limited to law enforcement activities or external security attacks, the IMEMC shall take reasonable measures which it deems appropriate, including internal investigation and, if required, cooperation with our web host. 
<P> 
If the IMEMC becomes aware that a user's personal information provided to IMEMC has been disclosed in a manner not permitted by this Privacy Policy, the IMEMC shall make reasonable efforts to notify the affected user, as soon as reasonably possible, of what information has been disclosed, to the extent that the IMEMC knows this information. 
<P> 
<b>Updates to the Privacy Policy</b> 
<P> 
To update the privacy policy, we will post the changed version and its effective date on this page 
<hr> 
<i>Last updated October 2006</i></DIV>
<!-- </privacy obj id 2042> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2042&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [privacy] Configuration" alt="Edit Static Document [privacy] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          