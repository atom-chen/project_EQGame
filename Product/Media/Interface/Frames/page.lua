--require('luaunit')

--function test()
	--Data = {1, 2, 3}
	--assertEquals(getTotalPage(Data, 1), 4)
	--assertEquals(getTotalPage(Data, 2), 2)
	--assertEquals(getPageCount(Data, 1, 2), 2)
	--assertEquals(getPageCount(Data, 2, 2), 1)
	--assertEquals(getData(Data, 1, 2), {1,2})
	--getData(Data, 2, 2)
--end


-- 获得页数
function getTotalPage(Data, MaxCount)
	return math.ceil(table.getn(Data) / MaxCount)
end

-- 获得每一页的最大条数
local function getPageCount(Data, Page, MaxCount)
	Size = table.getn(Data)
	S = Page * MaxCount
	if Size >=  S then
		return MaxCount
	else
		return Size % MaxCount
	end
end

-- 获得指定页的数据
function getData(Data, Page, MaxCount)
	if nil == Data then
		return nil
	end

	t = {}
	Start = (Page - 1) * MaxCount + 1
	End = math.min(Start - 1 + getPageCount(Data, Page, MaxCount), table.getn(Data))
	j = 1
	for i = Start, End do
		t[j] = Data[i]
		j = j + 1
	end
	return t
end


