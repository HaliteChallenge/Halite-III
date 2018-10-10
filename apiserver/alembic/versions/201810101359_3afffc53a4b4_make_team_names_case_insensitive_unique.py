"""Make team names case-insensitive unique

Revision ID: 3afffc53a4b4
Revises: eab38b06d0a0
Create Date: 2018-10-10 13:59:11.273843+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '3afffc53a4b4'
down_revision = 'eab38b06d0a0'
branch_labels = None
depends_on = None


def upgrade():
    op.create_index("uq_team_name", "team", [sa.text("lower(name)")])


def downgrade():
    op.drop_index("uq_team_name", "team")
