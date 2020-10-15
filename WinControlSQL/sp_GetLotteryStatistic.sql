USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLotteryStatistic]    Script Date: 10/15/2020 22:50:44 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GetLotteryStatistic]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GetLotteryStatistic]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetLotteryStatistic]    Script Date: 10/15/2020 22:50:44 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[sp_GetLotteryStatistic]
	@InUserControled int = 0,
	@InTypeID int = 1,
	@InBeginIssueNumber varchar(30) = '',
	@InCurrentIssueNumber varchar(30) = '',
	@InLastIssueNumber varchar(30) = ''
AS
BEGIN
	SET NOCOUNT ON
	
	--计算投注人数
	declare @UserCounts int = 0
	select @UserCounts = count(UserCounts) from 
			(select count(distinct UserID) UserCounts from [9lottery].[dbo].tab_GameOrder where IssueNumber = @InCurrentIssueNumber and TypeID = @InTypeID group by UserID) as t
	
	--计算区间投注金额 派彩金额
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @AllBetAsOfLast decimal(20, 2) = 0.0 --截止上期投注金额
	declare @WinRateAsOfLast decimal(20, 2) = 0.0 --截止上期玩家赢率
	select @AllBet = isnull(sum(RealAmount), 1) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InCurrentIssueNumber
	select @AllBetAsOfLast = isnull(sum(RealAmount), 1) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	select @BonusAlready = isnull(sum(ProfitAmount - RealAmount), 0) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	set @WinRateAsOfLast = isnull(@BonusAlready / @AllBetAsOfLast, 0)
	select @UserCounts, @AllBet, @AllBetAsOfLast, @BonusAlready, @WinRateAsOfLast
	--print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
	--print '截止上期投注金额@AllBetAsOfLast:' + isnull(cast(@AllBetAsOfLast as varchar(20)),0)
	--print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
	--print '截止上期赢率@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
END



















GO


