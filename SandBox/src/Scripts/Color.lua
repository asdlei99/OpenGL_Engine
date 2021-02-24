time = 0;

function ColorGradient(dt)
	time = time + dt;
	go = GameObject.This();
	gradientXZ = (math.cos(time) + 1) / 2;
	gradientY = (math.sin(time) + 1) / 2;
	go:SetColor(gradientXZ, gradientY, gradientXZ, 1);
end

function OnUpdate(dt)
	ColorGradient(dt);
end

function OnStart()
end