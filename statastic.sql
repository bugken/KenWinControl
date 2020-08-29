declare @UserControled int = 0
declare @ControlRate int = 0
declare @PeriodGap int = 0
select top 1 @UserControled = UserControled, @ControlRate = ControlRate, @PeriodGap = PeriodGap from caipiaos.dbo.tab_Game_Control order by UpdateTime desc
print '选取期数区间@PeriodGap:' + cast(@PeriodGap as varchar(10))
print '控制指数@ControlRate:' + cast(@ControlRate as varchar(10))
--单杀情况 单杀的信息需要写入到数据库

--计算区间投注金额 派彩金额,区间没有设置，默认当天作为区间
if (select ISNULL(GameUpdateNumberOpen,0) from caipiaos.dbo.tab_GameNumberSet)=1 and @ControlRate <> 0
begin
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @CurrentIssueNumber varchar(30) = ''
	declare @StartIssueNumber varchar(30) = ''
	declare @CurrentTime datetime = GETDATE()
	declare @LastPeriodTime datetime = DATEADD(mi,-3,GETDATE()) --上一期时间
	declare @DateTodayZero datetime = DATEADD(DAY,0,DATEDIFF(DAY,0,GETDATE()))
	declare @DateTomorrow datetime = DATEADD(DAY,1,DATEDIFF(DAY,0,GETDATE()))
	select top 1 @CurrentIssueNumber = IssueNumber from caipiaos.dbo.tab_Games where OptState!=1 and State=0 and StartTime<=@CurrentTime;
	if @CurrentIssueNumber = '' or @CurrentIssueNumber is null
		return --已经预设
	print '当前(终止)期数@CurrentIssueNumber:' + @CurrentIssueNumber 

	--declare @PeriodGap int = 10
	if @PeriodGap is not null and @PeriodGap <> 0 
	begin
		--对@PeriodGap进行判断处理
		declare @VarDay int = cast(substring(@CurrentIssueNumber, 0, 9) as int)
		declare @Period int = cast(substring(@CurrentIssueNumber, 9, 3) as int)
		if @PeriodGap < @Period
		begin 
			declare @TmpNumber bigint = cast(@CurrentIssueNumber as bigint) - @PeriodGap
			set @StartIssueNumber = cast(@TmpNumber as varchar(30))
		end
		else if @PeriodGap > @Period
		begin 
			declare @PeriodNotInToday int = @PeriodGap - @Period
			declare @DayNums int = 0 - (@PeriodNotInToday / 480 + 1)
			declare @PeriodNums int = 480 - @PeriodNotInToday % 480
			declare @TargetDate datetime = dateadd(Day, @DayNums, CONVERT(varchar(12), getdate(), 112))
			declare @VarTargetDate varchar(30) = CONVERT(varchar(12), @TargetDate, 112)
			if len(@PeriodNums) = 1
				set @StartIssueNumber = @VarTargetDate + '00' + cast(@PeriodNums as varchar(4))
			else if len(@PeriodNums) = 2
				set @StartIssueNumber = @VarTargetDate + '0' + cast(@PeriodNums as varchar(4))
			else
				set @StartIssueNumber = @VarTargetDate + cast(@PeriodNums as varchar(4))
		end
		print '起始期数@StartIssueNumber:' + @StartIssueNumber 
		select @AllBet = sum(RealAmount), @BonusAlready = sum(ProfitAmount - RealAmount) from caipiaos.dbo.tab_GameOrder where IssueNumber >= @StartIssueNumber and IssueNumber <= @CurrentIssueNumber
	end
	else
	begin 
		--默认按照一天计算
		print '默认区间为当天'
		select @AllBet = sum(RealAmount) from caipiaos.dbo.tab_GameOrder where SettlementTime between @DateTodayZero and @CurrentTime
		select @BonusAlready = sum(ProfitAmount - RealAmount) from caipiaos.dbo.tab_GameOrder where SettlementTime between @DateTodayZero and @LastPeriodTime
	end
	print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
	print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
	declare @CurrentRate decimal(6,3) = isnull(@BonusAlready/@AllBet, 0)
	print '当前赢率:' + cast(@CurrentRate as varchar(20))
end