
function deCapitalize( storyField)
{
    var myTitle;
    if (storyField == 1) myTitle = document.publishform.story_title.value;
    else if (storyField == 2) myTitle = document.publishform.comment_title.value;
    else if (storyField == 3) myTitle = document.publishform.story_summary.value;
    else if (storyField == 4) myTitle = document.publishform.story_content.value;
    else myTitle = document.publishform.comment_content.value;
    myTitle = myTitle.toLowerCase();

    var oneChar, prevChar ;
    var updateChar = 0;
    var found_fs   = 0;
    var t_str ;
    for(var i=0; i < myTitle.length; i++)
    {
        oneChar = myTitle.charAt(i);
        if (i == 0 )
	{
            updateChar = 1;
            oneChar = oneChar.toUpperCase();
	}
	else
	{
            prevChar = myTitle.charAt((i-1));
            if ((prevChar == ' ' || prevChar == '\n') && oneChar != ' ') 
            {
                if (storyField == 3 || storyField == 4 || storyField == 5) 
                {
                    if (found_fs == 1) 
                    {
		        updateChar = 1;
		        found_fs   = 0;
		        oneChar = oneChar.toUpperCase();
                    }
                }
	        else
                {
	            updateChar = 1;
	            oneChar = oneChar.toUpperCase();
                }
            }
	}
        if (updateChar == 1) 
        {
            if (i == 0 ) myTitle = oneChar + myTitle.substring(1);
	    else
            {
                myTitle = myTitle.substring(0,i) + oneChar + myTitle.substring(i+1);
	    }
            updateChar = 0; 
        }
        if (prevChar == '.') 
	{
            found_fs = 1; 
        } else {
            if (i > 4 )
	    {
                t_str = myTitle.substring((i-6),i);
                if (t_str == "<br />")
	        {
                    found_fs = 1; 
                }
            }
        }
    }
    if (storyField == 1) document.publishform.story_title.value = myTitle;
    else if (storyField == 2) document.publishform.comment_title.value = myTitle;
    else if (storyField == 3) document.publishform.story_summary.value = myTitle;
    else if (storyField == 4) document.publishform.story_content.value = myTitle;
    else document.publishform.comment_content.value = myTitle;
}
