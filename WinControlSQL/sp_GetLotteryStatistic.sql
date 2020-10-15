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
	
	--计算区间投注金额 派彩金额
	declare @UserCounts int = 0
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @AllBetAsOfLast decimal(20, 2) = 0.0 --截止上期投注金额
	declare @WinRateAsOfLast decimal(20, 2) = 0.0 --截止上期玩家赢率
	declare @BetCurrentIssue decimal(20, 2) = 0.0 --当期投注金额
	--使用top创建临时表,在临时表中读取数据效率高
	select top 5000 UserID, TypeID, IssueNumber, RealAmount, ProfitAmount into #tabGameOrder 
		from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID 
	select @UserCounts = count(UserCounts) from 
		(select count(distinct UserID) UserCounts from #tabGameOrder where IssueNumber = @InCurrentIssueNumber and TypeID = @InTypeID group by UserID) as t
	select @BetCurrentIssue = sum(RealAmount) from #tabGameOrder where TypeID = @InTypeID and IssueNumber = @InCurrentIssueNumber
	select @AllBetAsOfLast = sum(RealAmount), @BonusAlready = sum(ProfitAmount - RealAmount) from #tabGameOrder 
		where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	set @WinRateAsOfLast = @BonusAlready / @AllBetAsOfLast
	set @AllBet = @BetCurrentIssue + @AllBetAsOfLast
	drop table #tabGameOrder
	
	select @UserCounts, @AllBet, @AllBetAsOfLast, @BonusAlready, @WinRateAsOfLast
END






















GO


