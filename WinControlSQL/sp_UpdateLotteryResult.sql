USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_UpdateLotteryResult]    Script Date: 10/15/2020 16:50:43 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_UpdateLotteryResult]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_UpdateLotteryResult]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_UpdateLotteryResult]    Script Date: 10/15/2020 16:50:43 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[sp_UpdateLotteryResult]
	@RetID int = 0 output,
	@InTypeID int = 1,
	@InCurrentIssueNumber varchar(30) = '',
	@InNumber varchar(30) = '',
	@InColor varchar(30) = '',
	@InControlType int = 9
AS
BEGIN
	--总控制开关
	if (select ISNULL(GameUpdateNumberOpen,0) from [9lottery].dbo.tab_GameNumberSet) = 0
	begin
		--print '控制开关未开启'
		set @RetID = 100001 --控制开关未开启
		return
	end
	--单控开关
	if (select ISNULL(ControlEnabled,0) from [9lottery].dbo.tab_GameType where TypeID=@InTypeID) = 0
	begin
		--print '控制开关未开启或该期已经预设'
		set @RetID = 100002 --单控开关未开启
		return
	end
	--单个控制开关 预设
	declare @IsOpen int = 1
	declare @IsPreSetting int = 1
	declare @BeforePrenium varchar(20) = ''
	declare @BeforeSelectTypeNum varchar(5) = ''
	declare @BeforeSelectTypeColor varchar(5) = ''
	select @IsOpen=State, @IsPreSetting=OptState, @BeforePrenium=Premium, @BeforeSelectTypeNum=Number,@BeforeSelectTypeColor=Colour
		from [9lottery].dbo.tab_Games where TypeID = @InTypeID and IssueNumber = @InCurrentIssueNumber
	
	declare @RandNumVar varchar(20) = substring(@BeforePrenium, 0, 5) + @InNumber
	declare @Second varchar(4) = substring(CONVERT(varchar,GETDATE(),120), 18, 2)
	if @Second>='50' and @Second<'55' and @IsOpen=0 and @IsPreSetting=0  --50杀率计算开始,56秒前计算结束,57秒开始开奖结算 
	begin
		update [9lottery].dbo.tab_Games set Premium = @RandNumVar, Number = @InNumber, Colour = @InColor
			where TypeID = @InTypeID and IssueNumber = @InCurrentIssueNumber
		insert into [9lottery].dbo.tab_Game_Control_Log(TypeID, IssueNumber, OldPremium, OldNumber, OldColour, NewPremium, NewNumber, NewColour, ControlType, UpdateTime)
			values(@InTypeID, @InCurrentIssueNumber, @BeforePrenium, @BeforeSelectTypeNum, @BeforeSelectTypeColor, @RandNumVar, @InNumber, @InColor, @InControlType, getdate())
		set @RetID = 0
	end
	else
	begin
		set @RetID = 100003--未在更新时间内或已经预设或已经开奖
	end
END














GO


