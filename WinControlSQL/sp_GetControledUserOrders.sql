USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetControledUserOrders]    Script Date: 10/15/2020 22:51:05 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GetControledUserOrders]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GetControledUserOrders]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GetControledUserOrders]    Script Date: 10/15/2020 22:51:05 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO











CREATE PROCEDURE [dbo].[sp_GetControledUserOrders]
	@InUserControled int = 0,
	@InTypeID int = 1,
	@InBeginIssueNumber varchar(30) = '',
	@InCurrentIssueNumber varchar(30) = '',
	@InLastIssueNumber varchar(30) = ''
AS
BEGIN
	SET NOCOUNT ON
	
	--#单控玩家下注临时表
	create table #UserControledBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	--单人下注信息计算
	if @InUserControled > 0
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
		
		select TypeID, IssueNumber, SelectType, TotalBonus from #UserControledBonus order by TotalBonus desc
	end
	drop table #UserControledBonus
END


















GO


