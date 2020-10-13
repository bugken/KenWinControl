USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_QueryWinControl]    Script Date: 10/08/2020 19:02:30 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_QueryWinControl]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_QueryWinControl]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_QueryWinControl]    Script Date: 10/08/2020 19:02:30 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[sp_QueryWinControl]
	@InTypeID int = 1,
	@InIssueNumber varchar(50)
AS
BEGIN	
	--declare @InTypeID int = 1
	--declare @InIssueNumber varchar(50) = '2020101110038'
	--控制信息变量
	declare @UserControled int = 0
	declare @ControlRate int = 0
	declare @PeriodGap int = 0
	declare @PowerControl int = 0
	declare @Enabled int = 0
	declare @TypeIDFromTable int = 0
	--循环处理开奖
	declare @CurrentTime datetime = getdate()
	declare @CurrentTimeNextMin datetime = dateadd(minute,1,getdate())
	declare @OptState int = -1
	declare @IntervalM int = 0
	declare @CurrentIssueNumber varchar(60) = ''
	declare @BeginIssueNumber varchar(30) = ''
	declare @LastIssueNumber varchar(30) = ''
	declare @IssueStartTime datetime = getdate() --游戏开始时间
	declare @MinutesElapse int = datediff(minute, convert(datetime,convert(varchar(10),getdate(),120)), @CurrentTimeNextMin)--距离凌晨的分钟数
	--获取开奖期号
	select @CurrentIssueNumber=@InIssueNumber, @IntervalM=IntervalM, @OptState=OptState, @IssueStartTime=StartTime, @TypeIDFromTable=TypeID from  
			[9lottery].[dbo].tab_Games where IssueNumber=@InIssueNumber  
	--获取控制信息
	select top 1 @Enabled=ControlEnabled, @UserControled=ControledUserID, @ControlRate=isnull(ControlRate, 30), 
			@PeriodGap=isnull(ControlPeriodGap, 100), @PowerControl=ControlPower 
				from [9lottery].[dbo].tab_GameType where TypeID=@InTypeID order by TypeID
	if @PowerControl > 2
		set @PowerControl = 2
	else if @PowerControl < 0
		set @PowerControl = 0
	if @ControlRate <= 0
		set @ControlRate = 30
	print '受控用户@UserControled:' + cast(@UserControled as varchar(10))
	print '控制指数@ControlRate:' + cast(@ControlRate as varchar(10))
	print '选取期数区间@PeriodGap:' + cast(@PeriodGap as varchar(10))
	print '强弱控制@PowerControl:' + cast(@PowerControl as varchar(10))
	print '是否是能@Enabled:' + cast(@Enabled as varchar(10))
	--计算@LastIssueNumber @BeginIssueNumber
	set @LastIssueNumber = cast((cast(@CurrentIssueNumber as bigint)-1) as varchar(30))
	declare @VarDay int = cast(substring(@CurrentIssueNumber, 0, 9) as int)
	declare @TypeID int = cast(substring(@CurrentIssueNumber, 9, 1) as int)
	declare @Period int = cast(substring(@CurrentIssueNumber, 10, 4) as int)
	if @PeriodGap <= @Period
	begin 
		declare @TmpNumber bigint = cast(@CurrentIssueNumber as bigint) - @PeriodGap
		set @BeginIssueNumber = cast(@TmpNumber as varchar(30))
	end
	else if @PeriodGap > @Period
	begin 
		declare @BaseNum int = 24*60/@IntervalM
		declare @PeriodNotInToday int = @PeriodGap - @Period
		declare @DayNums int = 0 - (@PeriodNotInToday / @BaseNum + 1)
		declare @PeriodNums int = @BaseNum - @PeriodNotInToday % @BaseNum
		declare @TargetDate datetime = dateadd(Day, @DayNums, CONVERT(varchar(12), getdate(), 112))
		declare @VarTargetDate varchar(30) = CONVERT(varchar(12), @TargetDate, 112)
		if len(@PeriodNums) = 1
			set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) + '000' + cast(@PeriodNums as varchar(10))
		else if len(@PeriodNums) = 2
			set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) +'00' + cast(@PeriodNums as varchar(10))
		else if len(@PeriodNums) = 3
			set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) +'0' + cast(@PeriodNums as varchar(10))
		else
			set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) +cast(@PeriodNums as varchar(10))
	end
	print '当前期数@CurrentIssueNumber:' + @CurrentIssueNumber
	print '上一期数@LastIssueNumber:' + @LastIssueNumber
	print '起始期数@BeginIssueNumber:' + @BeginIssueNumber 
	print 'Game类型@TypeID:' + cast(@TypeID as varchar(2)) 
	print '是否预设@OptState:' + cast(@OptState as varchar(10)) 
	-------------------------------------------------------------------------------------------------------------------------
	-----------------------------------------------------分隔线--------------------------------------------------------------
	-------------------------------------------------------------------------------------------------------------------------
	declare @InUserControled int = 0
	declare @InControlRate int = @ControlRate
	declare @InPowerControl int = @PowerControl
	declare @InOptState int = @OptState
	declare @InBeginIssueNumber varchar(30) = @BeginIssueNumber
	declare @InCurrentIssueNumber varchar(30) = @CurrentIssueNumber
	declare @InLastIssueNumber varchar(30) = @LastIssueNumber
	--计算区间投注金额 派彩金额
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @AllBetUntilLast decimal(20, 2) = 0.0 --截止上期投注金额
	declare @WinRateAsOfLast decimal(20, 7) = 0.0 --截止上期玩家赢率
	select @AllBet = sum(RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InCurrentIssueNumber
	select @AllBetUntilLast = sum(RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	select @BonusAlready = sum(ProfitAmount - RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	set @WinRateAsOfLast = isnull(@BonusAlready / @AllBetUntilLast, 0)
	print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
	print '截止上期投注金额@AllBetUntilLast:' + isnull(cast(@AllBetUntilLast as varchar(20)),0)
	print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
	print '截止上期赢率@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
	--#LotteryTotalBonus记录输赢的临时表
	create table #LotteryTotalBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	create table #UserControledBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint, MultiRate decimal(2, 1))
	select UserId into #UserTest from [9lottery].dbo.tab_Users where UserType=1;
	insert into #LotteryTotalBonus(TypeID, IssueNumber, SelectType, TotalBonus, MultiRate)
		select @InTypeID, @InCurrentIssueNumber, SelectType, sum(RealAmount),
				case when SelectType in ('0','1','2','3','4','5','6','7','8','9') then 9
					 when SelectType in ('red','green','big','small') then 2
					 when SelectType = 'violet' then 5.5
				end
			from [9lottery].dbo.tab_GameOrder where UserID not in (Select UserID from #UserTest) and IssueNumber=@InCurrentIssueNumber 
				and TypeID=@InTypeID group by IssueNumber, SelectType
	--select TypeID, SelectType, IssueNumber, TotalBonus, MultiRate from #LotteryTotalBonus
	update #LotteryTotalBonus set TotalBonus *= MultiRate
	--select TypeID, SelectType, IssueNumber, TotalBonus from #LotteryTotalBonus
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
	--计算WinRate
	declare @TargetControlRate decimal(10,4) = (@InControlRate+0.0)/10000
	print '目标赢率@TargetControlRate:' + cast(@TargetControlRate as varchar(20))
	update #LotteryResult set WinRate = (isnull(@BonusAlready, 0)+AllTotalBonus)/@AllBet
	--更新游戏表并写入日志
	declare @RandNumVar varchar(20) = ''
	declare @Loops int = 0
	declare @IssueNumber varchar(50) = ''
	declare @WinRate decimal(10, 7) = 0.0
	declare @SelectTypeNum varchar(20) = ''
	declare @SelectTypeColor varchar(20) = ''
	declare @FinalTypeNum varchar(20) = ''
	declare @FinalTypeColor varchar(20) = ''
	declare @LogTypeNum varchar(20) = ''--用于记录Log
	declare @LogTypeColor varchar(20) = ''--用于记录Log
	declare @BeforeSelectTypeNum varchar(20) = ''
	declare @BeforeSelectTypeColor varchar(20) = ''
	declare @BeforePrenium varchar(20) = ''
	declare @TypeNum int = 0
	declare @PushUp bit = 0 --是否将用户赢率上拉向目标赢率靠近
	declare @BingoCounts int = 0
	declare @FirstLowWinRatePos int = 0 --第一个小值的位置
	declare @IsFound bit = 0 --是否继续找小值位置
	declare @StopPos int = 10 --强弱拉遍历停止位置
	declare @StepCounts int = 10 --遍历总数
	declare @IsUserControl bit = 0 --是否单控
	declare @UserControlType varchar(10) = '' --受控的类型
	declare @LogControlType int = 0 --Log类型 1:单杀,强拉,上拉 2:单杀,强拉,下拉 3:未单杀,强拉,上拉 4:未单杀,强拉,下拉 5:单杀,弱拉,上拉
									--6:未单杀,弱拉,上拉 7:单杀,弱拉,下拉 8:未单杀,弱拉,下拉 9:保持用户赢率为定值	
	if @WinRateAsOfLast < @TargetControlRate
		set @PushUp = 1
	if @PushUp = 1 and @InPowerControl = 1 --强上拉
		set @StopPos = 1
	if @PushUp = 0 and @InPowerControl = 1 --强下拉
		set @StopPos = 10

	select @BeforePrenium = Premium, @BeforeSelectTypeNum = Number, @BeforeSelectTypeColor = Colour from [9lottery].dbo.tab_Games where TypeID = @InTypeID and IssueNumber = @InCurrentIssueNumber
	select * from #LotteryResult order by WinRate desc
	
	declare CursorUpdate cursor for select IssueNumber, SelectTypeNum, SelectTypeColor, WinRate 
			from #LotteryResult where TypeID = @InTypeID ORDER BY WinRate desc
	open CursorUpdate
	fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
	while @@FETCH_STATUS = 0
	begin
		set @Loops = @Loops + 1
		if @InPowerControl > 0 --设置了强弱拉回
		begin
			--拉回到设定值(强弱两种方式)@PowerControl->1:强拉 2:弱拉
			if @InPowerControl = 1
			begin 
				if @StopPos = @Loops 
				begin 
					set @LogTypeNum = @SelectTypeNum
					set @LogTypeColor = @SelectTypeColor
					if @IsUserControl = 1 and @PushUp = 1
						set @LogControlType = 1 --单杀,强拉,上拉
					else if @IsUserControl = 1 and @PushUp = 0
						set @LogControlType = 2 --单杀,强拉,下拉
					else if @IsUserControl = 0 and @PushUp = 1
						set @LogControlType = 3 --未单杀,强拉,上拉
					else if @IsUserControl = 0 and @PushUp = 0
						set  @LogControlType = 4 --未单杀,强拉,下拉
					GOTO UpdateAndInsertLog --记录日志
				end
				else
				begin
					fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
					continue
				end
			end
			else if @InPowerControl = 2
			begin
				if @PushUp = 1	--上拉找大值
				begin
					if @Loops = 1 --@WinRate都比@@TargetControlRate小
					begin
						set @FinalTypeNum = @SelectTypeNum
						set @FinalTypeColor = @SelectTypeColor
					end
					--print '弱拉，上拉@WinRate:' + isnull(cast(@WinRate as varchar(20)),0)
					--print '弱拉，上拉@TargetControlRate:' + isnull(cast(@TargetControlRate as varchar(20)),0)
					if @WinRate > @TargetControlRate
					begin 
						set @FinalTypeNum = @SelectTypeNum
						set @FinalTypeColor = @SelectTypeColor
						set @BingoCounts = @BingoCounts + 1
					end
					if @BingoCounts = 3 or @Loops = @StepCounts --遍历到第三个大的值或结束
					begin
						--print '弱拉，上拉@BingoCounts:' + isnull(cast(@BingoCounts as varchar(20)),0) 
						--print '弱拉，上拉@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0)
						set @LogTypeNum = @FinalTypeNum
						set @LogTypeColor = @FinalTypeColor
						--print '弱拉，上拉@LogTypeNum:' + isnull(cast(@LogTypeNum as varchar(20)),0) 
						--print '弱拉，上拉@LogTypeColor:' + isnull(cast(@LogTypeColor as varchar(20)),0)
						if @IsUserControl = 1 
							set @LogControlType = 5 --单杀,弱拉,上拉
						else 
							set @LogControlType = 6 --未单杀,弱拉,上拉
						GOTO UpdateAndInsertLog --记录日志
					end
					fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
					continue
				end
				else	--下拉找小值
				begin
					--print '弱拉，下拉@WinRate:' + isnull(cast(@WinRate as varchar(20)),0)
					--print '弱拉，下拉@TargetControlRate:' + isnull(cast(@TargetControlRate as varchar(20)),0)
					if @WinRate < @TargetControlRate and @IsFound = 0  
					begin 
						set @FirstLowWinRatePos = @Loops
						set @IsFound = 1
						--print '弱拉，下拉@FirstLowWinRatePos:' + isnull(cast(@FirstLowWinRatePos as varchar(20)),0) 
						if @FirstLowWinRatePos >= @StepCounts - 2
						begin
							--print '弱拉，下拉@FirstLowWinRatePos:' + isnull(cast(@FirstLowWinRatePos as varchar(20)),0) 
							--print '弱拉，下拉@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0) 
							set @LogTypeNum = @SelectTypeNum
							set @LogTypeColor = @SelectTypeColor
							--print '弱拉，下拉@LogTypeNum:' + isnull(cast(@LogTypeNum as varchar(20)),0) 
							--print '弱拉，下拉@LogTypeColor:' + isnull(cast(@LogTypeColor as varchar(20)),0) 
							if @IsUserControl = 1 
								set @LogControlType = 7 --单杀,弱拉,下拉
							else 
								set @LogControlType = 8 --未单杀,弱拉,下拉
							GOTO UpdateAndInsertLog --记录日志
						end
						else
							set @StopPos = @StepCounts -2
					end
					if (@IsFound=1 and @Loops=@StopPos) or (@IsFound=0 and @Loops=@StepCounts) 
					begin
						set @LogTypeNum = @SelectTypeNum
						set @LogTypeColor = @SelectTypeColor
						--print '1弱拉，下拉@FirstLowWinRatePos:' + isnull(cast(@FirstLowWinRatePos as varchar(20)),0) 
						--print '1弱拉，下拉@StopPos:' + isnull(cast(@StopPos as varchar(20)),0) 
						--print '1弱拉，下拉@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0) 
						--print '1弱拉，下拉@LogTypeNum:' + isnull(cast(@LogTypeNum as varchar(20)),0) 
						--print '1弱拉，下拉@LogTypeColor:' + isnull(cast(@LogTypeColor as varchar(20)),0)
						if @IsUserControl = 1 
							set @LogControlType = 7 --单杀,弱拉,下拉
						else 
							set @LogControlType = 8 --未单杀,弱拉,下拉
						GOTO UpdateAndInsertLog --记录日志
					end
					fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
					continue
				end
			end
		end
		else
		begin
			--保持用户赢率在设定值
			if @Loops = @StepCounts
			begin
				--print '控制杀率到固定值@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0)
				set @LogTypeNum = @SelectTypeNum
				set @LogTypeColor = @SelectTypeColor
				set @LogControlType = 9 --保持用户赢率为定值
				GOTO UpdateAndInsertLog --记录日志
			end
			if @WinRate <= @TargetControlRate
			begin
				--print '1控制杀率到固定值@WinRate:' + isnull(cast(@WinRate as varchar(20)),0)
				--print '1控制杀率到固定值@TargetControlRate:' + isnull(cast(@TargetControlRate as varchar(20)),0)
				set @LogTypeNum = @SelectTypeNum
				set @LogTypeColor = @SelectTypeColor
				set @LogControlType = 9 --保持用户赢率为定值
				GOTO UpdateAndInsertLog --记录日志
			end
			fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
			continue
		end
	UpdateAndInsertLog:
		print 'UpdateAndInsertLog' 
		--set @RandNumVar = substring(@BeforePrenium, 0, 5) + @LogTypeNum
		--insert into [9lottery].dbo.tab_Game_Control_Log(TypeID, IssueNumber, OldPremium, OldNumber, OldColour, NewPremium, NewNumber, NewColour, ControlType, UpdateTime)
		--	values(@InTypeID, @IssueNumber, @BeforePrenium, @BeforeSelectTypeNum, @BeforeSelectTypeColor, @RandNumVar, @LogTypeNum, @LogTypeColor, @LogControlType, getdate())
		break
	end

	close CursorUpdate
	deallocate CursorUpdate
	
	drop table #LotteryTotalBonus
	drop table #UserControledBonus
	drop table #UserTest
	drop table #LotteryResult
		
END

















GO


