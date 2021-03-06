USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLotteryUserOrders]    Script Date: 10/15/2020 03:27:51 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GetLotteryUserOrders]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GetLotteryUserOrders]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLotteryUserOrders]    Script Date: 10/15/2020 03:27:51 ******/
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
	SET NOCOUNT ON

	--计算区间投注金额 派彩金额
	declare @UserCounts int = 0
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @AllBetAsOfLast decimal(20, 2) = 0.0 --截止上期投注金额
	declare @WinRateAsOfLast decimal(20, 2) = 0.0 --截止上期玩家赢率
	declare @BetCurrentIssue decimal(20, 2) = 0.0 --当期投注金额
	--使用top创建临时表,在临时表中读取数据效率高
	select UserID, TypeID, IssueNumber, RealAmount, ProfitAmount, SelectType into #tabGameOrder 
		from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InCurrentIssueNumber
	select @UserCounts = count(UserCounts) from 
		(select count(distinct UserID) UserCounts from #tabGameOrder where IssueNumber = @InCurrentIssueNumber group by UserID) as t
	select @BetCurrentIssue = sum(RealAmount) from #tabGameOrder where IssueNumber = @InCurrentIssueNumber
	select @AllBetAsOfLast = sum(RealAmount), @BonusAlready = sum(ProfitAmount - RealAmount) from #tabGameOrder where IssueNumber <= @InLastIssueNumber
	if @AllBetAsOfLast=0
		set @AllBetAsOfLast=1
	set @WinRateAsOfLast = @BonusAlready / @AllBetAsOfLast
	set @AllBet = @BetCurrentIssue + @AllBetAsOfLast
	select @UserCounts, @AllBet, @AllBetAsOfLast, @BonusAlready, @WinRateAsOfLast
	--print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
	--print '截止上期投注金额@AllBetAsOfLast:' + isnull(cast(@AllBetAsOfLast as varchar(20)),0)
	--print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
	--print '截止上期赢率@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
	
	--#LotteryTotalBonus记录输赢的临时表
	create table #LotteryTotalBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	create table #UserTest(UserID int)
	insert into #UserTest select UserId from tab_Users where UserType=1;
	--insert into #UserTest select @InUserControled --TODO:如果有单控,是否需要将单控玩家下注去掉 
	insert into #LotteryTotalBonus(TypeID, IssueNumber, SelectType, TotalBonus, MultiRate)
		select @InTypeID, @InCurrentIssueNumber, SelectType, sum(RealAmount),
				case when SelectType in ('0','1','2','3','4','5','6','7','8','9') then 9
					 when SelectType in ('red','green','big','small') then 2
					 when SelectType = 'violet' then 5.5
				end
			from #tabGameOrder  where UserID not in (Select UserID from #UserTest) 
				and IssueNumber=@InCurrentIssueNumber group by IssueNumber, SelectType
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
	update #LotteryResult set WinRate = (isnull(@BonusAlready, 0)+AllTotalBonus)/@AllBet 
	
	--单杀下注信息计算
	create table #UserControledBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	if @InUserControled <> 0
	begin 
		insert into #UserControledBonus(TypeID, IssueNumber, SelectType, TotalBonus, MultiRate)
			select @InTypeID, @InCurrentIssueNumber, SelectType, sum(RealAmount),
					case when SelectType in ('0','1','2','3','4','5','6','7','8','9') then 9
						 when SelectType in ('red','green','big','small') then 2
						 when SelectType = 'violet' then 5.5
					end 
				from #tabGameOrder
					where IssueNumber=@InCurrentIssueNumber 
						and UserID = @InUserControled group by IssueNumber, SelectType
		update #UserControledBonus set TotalBonus *= MultiRate
	end
	
	select TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate from #LotteryResult order by WinRate desc
	select TypeID, IssueNumber, SelectType, TotalBonus from #UserControledBonus order by TotalBonus desc
	
	drop table #LotteryTotalBonus
	drop table #UserControledBonus
	drop table #LotteryResult
	drop table #UserTest
	drop table #tabGameOrder
END















GO


