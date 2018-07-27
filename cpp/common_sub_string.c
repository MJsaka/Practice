#include <stdio.h>
#include <sys/stat.h>

int LongestCommonSubString(const char *str1, const char *str2)
{
	if ( str1==NULL || str2==NULL )
	{
		return 0;
	}

	int len1 = strlen(str1);
	int len2 = strlen(str2);

	int start1 = -1;
	int start2 = -1;
	int longest = 0;

	if ( len1 == 0 || len2 == 0 )
	{
		return 0;
	}

	int i,j;
	for ( int m = 0 ; m < len1 + len2 - 1; ++m)
	{
		if ( m < len1) {
			i = m; j = 0;
		} else {
			i = 0; j = m - len1 + 1;
		}

		int length = 0;

		if (len1-i <= longest || len2-j <= longest)
		{
			break;
		}

		while(i < len1 && j < len2)
		{
			if (str1[i] != str2[j])
			{
				length = 0;
			}
			else 
			{
				++length;
				if (longest < length)
				{
					longest = length;
					start1 = i-longest+1;
					start2 = j-longest+1;
				}
			}

			++i;
			++j;
		} 
	}

	printf("start1:%d, start2:%d\n",start1,start2);
	printf("the longest common sub string is\n");
	for (int m = 0; m < longest; ++m)
		fputc(str1[start1+m], stdout);
	printf("\n");

	fflush(stdout);

	return longest;
}

int main()
{
	int const buf_len = 40960;
	char buf1[buf_len];
	char buf2[buf_len];

	printf("this program cal the longest common sub string of two string\n");

	int fd = fileno(stdin);
	int stdinIsFile = 0;
	struct stat stat_buf;
	if ( fstat(fd, &stat_buf) == 0 )
	{
		if ( S_ISREG(stat_buf.st_mode) )
			stdinIsFile = 1;
	}

	if ( !stdinIsFile )
	{
		printf("please input str1:");
		fflush(stdout);
	}

	fgets(buf1, buf_len, stdin);
	int len1 = strlen(buf1);
	buf1[len1-1] = '\0';

	if ( !stdinIsFile )
	{
		printf("please input str2:");
		fflush(stdout);
	}

	fgets(buf2, buf_len, stdin);
	int len2 = strlen(buf2);
	buf2[len2-1] = '\0';

	printf("str1:%s\n",buf1);
	printf("str2:%s\n",buf2);

	int n = LongestCommonSubString(buf1, buf2);	
	printf("len of longest common sub string is : %d\n",n);
}

