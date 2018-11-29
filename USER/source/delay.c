/*******************************************************************************
* Function Name : Delay
* Description   : delay
* Input         : t.
* Output        : None.
* Return        : None.
*******************************************************************************/
void Delay(unsigned int t)
{
   unsigned int i;
   while(t>0)
   {
      for(i=1000;i>0;i--);
	  t--;
      IWDG_ReloadCounter();
   }
}
