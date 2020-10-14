USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLotteryUserOrders]    Script Date: 10/15/2020 01:51:06 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GetLotteryUserOrders]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GetLotteryUserOrders]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLotteryUserOrders]    Script Date: 10/15/2020 01:51:06 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[sp_GetLotteryUserOrders]
	@InUserControled int = 0,
	@InTypeID int = 1,
	@InBeginIssueNumber varchar(30) = '',
	@InCurrentIssueNumber varchar(30) = '',
	@InLastIssueNumber varchar(30) = ''
AS
BEGIN
	--控制开关未开启或者已经预设,不进行控制
	if (select ISNULL(GameUpdateNumberOpen,0) from [9lottery].dbo.tab_GameNumberSet) = 0
	begin
		--print '控制开关未开启或该期已经预设'
		return
	end
	
	--投注人数不得少于5，否则不调控
	declare @UserCounts int = 0
	select @UserCounts = count(UserCounts) from 
			(select count(distinct UserID) UserCounts from [9lottery].[dbo].tab_GameOrder where IssueNumber = @InCurrentIssueNumber and TypeID = @InTypeID group by UserID) as t
	if @UserCounts <= 5
	begin
		--print '下注人数小于5个人'
		return
	end
	
	--计算区间投注金额 派彩金额
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @AllBetAsOfLast decimal(20, 2) = 0.0 --截止上期投注金额
	declare @WinRateAsOfLast decimal(20, 2) = 0.0 --截止上期玩家赢率
	select @AllBet = sum(RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InCurrentIssueNumber
	select @AllBetAsOfLast = sum(RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	select @BonusAlready = sum(ProfitAmount - RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	set @WinRateAsOfLast = isnull(@BonusAlready / @AllBetAsOfLast, 0)
	select @UserCounts, @AllBet, @AllBetAsOfLast, @BonusAlready, @WinRateAsOfLast
	--print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
	--print '截止上期投注金额@AllBetAsOfLast:' + isnull(cast(@AllBetAsOfLast as varchar(20)),0)
	--print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
	--print '截止上期赢率@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
	
	--#LotteryTotalBonus记录输赢的临时表
	create table #LotteryTotalBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	create table #UserControledBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
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
	
	--单人下注信息计算
	if @InUserControled <> 0
	begin 
		insert into #UserControledBonus(TypeID, IssueNumber, SelectType, TotalBonus, MultiRate)
			select @InTypeID, @InCurrentIssueNumber, SelectType, sum(RealAmount),
					case when SelectType in ('0','1','2','3','4','5','6','7','8','9') then 9
						 when SelectType in ('red','green','big','small') then 2
						 when SelectType = 'violet' then 5.5
					end 
				from [9lottery].dbo.tab_GameOrder 
					where IssueNumber=@InCurrentIssueNumber and TypeID=@InTypeID and UserID = @InUserControled
						group by IssueNumber, SelectType
		update #UserControledBonus set TotalBonus *= MultiRate
		
	end
	
	--玩家没有下注，直接返回
	declare @BetCounts int = 0
	select @BetCounts = count(*) from #LotteryTotalBonus
	if @BetCounts = 0
	begin
		--print '玩家没有下注'
		drop table #LotteryTotalBonus
		drop table #UserControledBonus
		drop table #UserTest
		return
	end
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
	update #LotteryResult set WinRate = (isnull(@BonusAlready, 0)+AllTotalBonus)/@AllBet 
	
	select TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate from #LotteryResult order by WinRate
	select TypeID, IssueNumber, SelectType, TotalBonus from #UserControledBonus order by TotalBonus desc
	
	drop table #LotteryTotalBonus
	drop table #UserControledBonus
	drop table #LotteryResult
	drop table #UserTest
END













GO


