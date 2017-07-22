--A*数学
local math = math
local M = {}

--向量长度
function M.vectorMagnitude(x, y)
	return math.sqrt(x * x + y * y)
end

--向量单位化
function M.vectorNormalized(x, y)
	local b1 = M.vectorMagnitude(x, y)
	local c1 = 1/b1
	
	return c1*x, c1*y
end

--两向量间的夹角
function M.Angle(v1x, v1y, v2x, v2y)
	return math.acos(v1x*v2x+v1y*v2y)/(M.vectorMagnitude(v1x, v1y) * M.vectorMagnitude(v2x, v2y))*180/math.pi
end

--浮点相等
function M.approximatelyF32(a1, b1)
	local diff = a1 - b1
	return diff > -0.0001 and diff < 0.0001
end

--舍入
function M.Round(f)
	local r = math.floor(f)
	if f-r ~= .5 then
		return math.floor(f + .5)
	else
		if r%2 == 0 then
			return r
		else
			return r + 1
		end
	end
end

function M.clampInt(value, minv, maxv)
	if value < minv then
		return minv
	end

	if value > maxv then
		return maxv
	end

	return value
end

return M
