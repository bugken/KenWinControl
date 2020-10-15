USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLottery10Results]    Script Date: 10/15/2020 22:51:47 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GetLottery10Results]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GetLottery10Results]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLottery10Results]    Script Date: 10/15/2020 22:51:47 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[sp_GetLottery10Results]
	@BonusAlready bigint = 0, 
	@AllBet bigint = 0, 
	@InUserControled int = 0,
	@InTypeID int = 1,
	@InBeginIssueNumber varchar(30) = '',
	@InCurrentIssueNumber varchar(30) = '',
	@InLastIssueNumber varchar(30) = ''
AS
BEGIN
	SET NOCOUNT ON
	
	--#LotteryTotalBonus记录输赢的临时表
	create table #LotteryTotalBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	select UserId into #UserTest from tab_Users where UserType=1;
	insert into #LotteryTotalBonus(TypeID, IssueNumber, SelectType, TotalBonus, MultiRate)
		select @InTypeID, @InCurrentIssueNumber, SelectType, sum(RealAmount),
				case when SelectType in ('0','1','2','3','4','5','6','7','8','9') then 9
					 when SelectType in ('red','green','big','small') then 2
					 when SelectType = 'violet' then 5.5
				end
			from [9lottery].dbo.tab_GameOrder where UserID not in (Select UserID from #UserTest) and IssueNumber=@InCurrentIssueNumber 
				and TypeID=@InTypeID group by IssueNumber, SelectType
	update #LotteryTotalBonus set TotalBonus *= MultiRate
	
	--select TypeID, SelectType, IssueNumber, TotalBonus from #LotteryTotalBonus
	--select * from #UserControledBonus
	
	--获取彩票所有可能出现的结果
	create table #LotteryResult(TypeID int, IssueNumber varchar(50), SelectTypeNum varchar(20), SelectTypeColor varchar(20), AllTotalBonus bigint, WinRate decimal(10, 7))
	insert into #LotteryResult(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
		select @InTypeID, @InCurrentIssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, 0.0 from [9lottery].dbo.tab_Game_Result
	--算出11种结果对应的派彩(10 violet)
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1
		inner join #LotteryTotalBonus t2 on t1.SelectTypeNum = t2.SelectType --or t1.SelectTypeColor = t2.SelectType
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1
		inner join #LotteryTotalBonus t2 on t1.SelectTypeColor = t2.SelectType --or t1.SelectTypeColor = t2.SelectType
	--加入大小下注的派彩
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1  
		inner join #LotteryTotalBonus t2 on t2.SelectType='big' and t1.SelectTypeNum in ('5','6','7','8','9')
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1  
		inner join #LotteryTotalBonus t2 on t2.SelectType='small' and t1.SelectTypeNum in ('0','1','2','3','4')
	--加上violet下注的派彩并删除violet记录
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1  
		inner join #LotteryTotalBonus t2 on t2.SelectType='violet' and t1.SelectTypeNum in ('0','5')
	delete from #LotteryResult where SelectTypeColor = 'violet' and SelectTypeNum = '10'
	--更改结果的颜色
	update #LotteryResult SET SelectTypeColor = (case SelectTypeNum when '0' then 'red,violet' when '5' then 'green,violet' else SelectTypeColor end)
	--计算赢率
	update #LotteryResult set WinRate = (@BonusAlready+AllTotalBonus+0.0)/@AllBet 
	
	select TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate from #LotteryResult order by WinRate desc
	
	drop table #LotteryTotalBonus
	drop table #LotteryResult
	drop table #UserTest
END


















GO


